#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"

bool gDepthWrite = true;
GameLib::Framework::BlendMode gBlendMode = GameLib::Framework::BLEND_LINEAR;
bool gSwapOrder = false;

namespace GameLib{
	void Framework::update(){
		setFrameRate( 60 );

		enableDepthTest( true ); //Zテストはon
		enableDepthWrite( gDepthWrite ); //ここで変えるのはZバッファへの書き込み
		setBlendMode( gBlendMode );

		double point0[ 3 ] = { 200.0, 100.0, 1.0 };
		double point1[ 3 ] = { 400.0, 100.0, 1.0 };
		double point2[ 3 ] = { 300.0, 400.0, 0.0 };
		unsigned c0 = 0x80ff8080; //赤

		double point3[ 3 ] = { 200.0, 400.0, 1.0 };
		double point4[ 3 ] = { 400.0, 400.0, 1.0 };
		double point5[ 3 ] = { 300.0, 100.0, 0.0 };
		unsigned c1 = 0x8080ff80; //緑

		if ( gSwapOrder ){
			drawTriangle3D( point3, point4, point5, 0, 0, 0, c1, c1, c1 );
			drawTriangle3D( point0, point1, point2, 0, 0, 0, c0, c0, c0 );
		}else{
			drawTriangle3D( point0, point1, point2, 0, 0, 0, c0, c0, c0 );
			drawTriangle3D( point3, point4, point5, 0, 0, 0, c1, c1, c1 );
		}

		if ( Input::Manager::instance().keyboard().isTriggered( 'a' ) ){
			gSwapOrder = !gSwapOrder;
		}
		if ( Input::Manager::instance().keyboard().isTriggered( 's' ) ){
			if ( gBlendMode == BLEND_ADDITIVE ){
				gBlendMode = BLEND_LINEAR;
			}else{
				gBlendMode = BLEND_ADDITIVE;
			}
		}
		if ( Input::Manager::instance().keyboard().isTriggered( 'd' ) ){
			gDepthWrite = !gDepthWrite;
		}

		if ( gSwapOrder ){
			drawDebugString( 0, 0, "[a] Swap Order : On" );
		}else{
			drawDebugString( 0, 0, "[a] Swap Order : Off" );
		}
		if ( gBlendMode == BLEND_LINEAR ){
			drawDebugString( 0, 1, "[s] Linear Blend" );
		}else{
			drawDebugString( 0, 1, "[s] Additive Blend" );
		}
		if ( gDepthWrite ){
			drawDebugString( 0, 2, "[d] Depth Write : On" );
		}else{
			drawDebugString( 0, 2, "[d] Depth Write : Off" );
		}
	}
}
