#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
using namespace GameLib;
#include "Image.h"
#include "Matrix23.h"
#include "Vector2.h"
#include "GameLib/Math.h"

Image* gImage;
Texture* gTexture;
bool gFirstFrame = true;
bool gScaleFirst = true;
int gImageWidth;
int gImageHeight;
int gCount = 0;

namespace GameLib{
	void Framework::update(){
		sleep( 16 );
		if ( gFirstFrame ){
			gFirstFrame = false;
			gImage = new Image( "background.dds" ); 
			gImageWidth = gImage->width();
			gImageHeight = gImage->height();
			createTexture( 
				&gTexture,
				gImageWidth, 
				gImageHeight, 
				gImage->data(), 
				gImageWidth, 
				gImageHeight );
			SAFE_DELETE( gImage );
		}
		setTexture( gTexture );
		
		double rotation = static_cast< double >( gCount );
		Vector2 scale( sin( rotation ) + 0.5, cos( rotation ) + 0.5 );

		//行列作る
		Matrix23 m;
		m.setTranslation( Vector2( gImageWidth/2, gImageHeight/2 ) );
		if ( gScaleFirst ){
			m.rotate( rotation );
			m.scale( scale );
		}else{
			m.scale( scale );
			m.rotate( rotation );
		}
		m.translate( Vector2( -gImageWidth/2, -gImageHeight/2 ) );

		Vector2 p0( 0.0, 0.0 );
		Vector2 p1( 100.0, 0.0 );
		Vector2 p2( 0.0, 100.0 );
		Vector2 p3( 100.0, 100.0 );
		double t0[ 2 ] = { 0.0, 0.0 };
		double t1[ 2 ] = { 1.0, 0.0 };
		double t2[ 2 ] = { 0.0, 1.0 };
		double t3[ 2 ] = { 1.0, 1.0 };
		//行列乗算
		m.multiply( &p0, p0 );
		m.multiply( &p1, p1 );
		m.multiply( &p2, p2 );
		m.multiply( &p3, p3 );
		//描画
		//Vector2はそのままは渡せません。
		drawTriangle2D( &p0.x, &p1.x, &p2.x, t0, t1, t2 ); //012
		drawTriangle2D( &p3.x, &p1.x, &p2.x, t3, t1, t2 ); //312

		//スペースで切り替え
		if ( Input::Manager::instance().keyboard().isTriggered( ' ' ) ){
			gScaleFirst = !gScaleFirst;
		}
		++gCount;

		drawDebugString( 0, 0, "press SPACE to swap ROTATION and SCALING" );
		//終了処理してみようか
		if ( isEndRequested() ){
			destroyTexture( &gTexture );
		}
	}
}
