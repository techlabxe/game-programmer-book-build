#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"

#include <sstream>
using namespace std;

double gZMove = 2000.0;
bool gFirstFrame = true;
GameLib::Texture* gTexture;

namespace GameLib{
	void Framework::update(){
		setFrameRate( 60 );
		if ( gFirstFrame ){
			gFirstFrame = false;
			createTexture( &gTexture, "grid.tga" ); 
		}

		double p[ 4 ][ 4 ] = { //4次元 
			{ -500.0, -300.0, -1000.0, 1.0 }, //Yは上がプラスなので下にずらすにはマイナス
			{ -500.0, -300.0, 1000.0, 1.0 },
			{ 500.0, -300.0, -1000.0, 1.0 },
			{ 500.0, -300.0, 1000.0, 1.0 },
		};
		double uv[ 4 ][ 2 ] = { 
			{ 0.0, 0.0 },
			{ 1.0, 0.0 },
			{ 0.0, 1.0 },
			{ 1.0, 1.0 },
		};

		//ハードウェアに渡す準備
		for ( int i = 0; i < 4; ++i ){
			//yに640/480をかけて縦横比を調整
			p[ i ][ 1 ] *= 640.0 / 480.0;
			//移動量反映
			p[ i ][ 2 ] += gZMove;
			//wに範囲変換前のzを格納
			p[ i ][ 3 ] = p[ i ][ 2 ];
			//z範囲変換
			p[ i ][ 2 ] = ( 1.0/9999.0 ) * p[ i ][ 2 ] - ( 1.0/9999.0 );
			//zに範囲変換前のzを余分に乗算
			p[ i ][ 2 ] *= p[ i ][ 3 ];
		}
		//四角形を描く。
		setTexture( gTexture );
		drawTriangle3DH( p[ 0 ], p[ 1 ], p[ 2 ], uv[ 0 ], uv[ 1 ], uv[ 2 ], 0xffff0000, 0xffff0000, 0xffff0000 );
		drawTriangle3DH( p[ 3 ], p[ 1 ], p[ 2 ], uv[ 3 ], uv[ 1 ], uv[ 2 ], 0xff00ff00, 0xff00ff00, 0xff00ff00 );

		if ( Input::Manager::instance().keyboard().isOn( 'z' ) ){
			gZMove -= 10.f;
		}else if ( Input::Manager::instance().keyboard().isOn( 'w' ) ){
			gZMove += 10.f; 
		}
		//表示
		ostringstream oss;
		oss << "[w-z] Z MOVEMENT: " << gZMove;
		drawDebugString( 10, 0, oss.str().c_str() );
		oss.str( "" );

		if ( isEndRequested() ){
			destroyTexture( &gTexture );
		}
	}
}
