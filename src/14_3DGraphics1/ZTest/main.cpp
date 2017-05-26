#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"

bool gDepthTest = true;

namespace GameLib{
	void Framework::update(){
		setFrameRate( 60 );
		enableDepthTest( gDepthTest );

		double point0[ 3 ] = { 200.0, 100.0, 1.0 };
		double point1[ 3 ] = { 400.0, 100.0, 1.0 };
		double point2[ 3 ] = { 300.0, 400.0, 0.0 };
		unsigned c = 0xffff0000; //ê‘
		drawTriangle3D( point0, point1, point2, 0, 0, 0, c, c, c );

		double point3[ 3 ] = { 200.0, 400.0, 1.0 };
		double point4[ 3 ] = { 400.0, 400.0, 1.0 };
		double point5[ 3 ] = { 300.0, 100.0, 0.0 };
		c = 0xff00ff00; //óŒ
		drawTriangle3D( point3, point4, point5, 0, 0, 0, c, c, c );

		if ( Input::Manager::instance().keyboard().isTriggered( ' ' ) ){
			gDepthTest = !gDepthTest;
		}
		if ( gDepthTest ){
			drawDebugString( 0, 0, "Depth Test : On (press space)" );
		}else{
			drawDebugString( 0, 0, "Depth Test : Off (press space)" );
		}
	}
}
