#include <sstream>
#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "Image.h"
#include "Vector2.h"
#include "Matrix23.h"
#include "GameLib/GameLibMath.h"
using namespace std;

namespace GameLib {
	int round(double a) {
		a += (a > 0.0) ? 0.5 : -0.5f;
		return static_cast<int>(a);
	}
}

bool gFirstFrame = true;
Image* gImage;
int gCount;

namespace GameLib{
	void Framework::update(){
		if ( gFirstFrame ){
			gImage = new Image( "background.dds" );
			gFirstFrame = false;
		}
		unsigned* vram = videoMemory();
		int ww = width(); //window width
		int wh = height(); //window height
		//一旦全部真っ黒に
		for ( int i = 0; i < ww * wh; ++i ){
			vram[ i ] = 0;
		}
		int iw = gImage->width(); //image width
		int ih = gImage->height(); //image height

		//行列を作ろう
		double rotation = static_cast< double >( gCount );
		//画像中心を原点に、拡大、回転、中心を戻す、の順。
		Matrix23 translationMatrix1;
		Matrix23 scalingMatrix;
		Matrix23 rotationMatrix;
		Matrix23 translationMatrix2;
		//移動1(画像中心を原点に)
		translationMatrix1.setTranslation( Vector2( -iw/2, -ih/2 ) );
		//拡大
		Vector2 scale( sin( rotation )*2.0 + 1.0, cos( rotation )*2.0 + 1.0 );
		scalingMatrix.setScaling( Vector2( scale.x, scale.y ) );
		//回転
		rotationMatrix.setRotation( rotation );
		//戻す
		translationMatrix2.setTranslation( Vector2( iw/2, ih/2 ) );
		//行列の統合(逆順)
		Matrix23 transform;
		transform = translationMatrix2;
		transform *= rotationMatrix;
		transform *= scalingMatrix;
		transform *= translationMatrix1;

		//3点作る
		Vector2 a, b, c;
		transform.multiply( &a, Vector2( 0, 0 ) );
		transform.multiply( &b, Vector2( iw, 0 ) );
		transform.multiply( &c, Vector2( 0, ih ) );
		//b-a,c-aを計算
		Vector2 ab, ac;
		ab.setSub( b, a );
		ac.setSub( c, a );
		//補間開始
		double rcpWidth = 1.0 / static_cast< double >( iw );
		double rcpHeight = 1.0 / static_cast< double >( ih );
		for ( int y = 0; y < ih; ++y ){
			double yf = static_cast< double >( y ) + 0.5;
			double v = yf * rcpHeight;
			for ( int x = 0; x < iw; ++x ){
				double xf = static_cast< double >( x ) + 0.5;
				double u = xf * rcpWidth;
				Vector2 p;
				p.setInterporation( a, ab, ac, u, v );
				p -= Vector2( 0.5, 0.5 ); //添え字へ
				int rx, ry;
				rx = round( p.x );
				ry = round( p.y );
				//範囲内なら貼り付け
				if ( rx >= 0 && rx < ww && ry >= 0 && ry < wh ){
					vram[ ry * ww + rx ] = gImage->pixel( x, y );
				}
			}
		}
		++gCount;

		ostringstream oss;
		oss << frameRate();
		drawDebugString( 0, 0, oss.str().c_str() );
	}
}
