#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include <sstream>
using namespace std;

double gZMove = -999.0;
double gXMove = -0.0;

namespace GameLib{
	void Framework::update(){
		setFrameRate( 60 );

		double p[ 4 ][ 3 ] = { 
			{ 1500.0, 500.0, 1000.0 },
			{ 500.0, 1500.0, 1000.0 },
			{ 1500.0, 500.0, 2000.0 },
			{ 500.0, 1500.0, 2000.0 },
		};

		//x,yをzで割り、zを[1,1000]から[0,1]に範囲変換する。
		for ( int i = 0; i < 4; ++i ){
			//移動量加算
			p[ i ][ 0 ] += gXMove;
			p[ i ][ 2 ] += gZMove;
			p[ i ][ 0 ] /= p[ i ][ 2 ];
			p[ i ][ 1 ] /= p[ i ][ 2 ];
			p[ i ][ 2 ] = ( 1.0/9999.0 ) * p[ i ][ 2 ] - ( 1.0/9999.0 );
		}
		//四角形を描く。
		drawTriangle3D( p[ 0 ], p[ 1 ], p[ 2 ] );
		drawTriangle3D( p[ 3 ], p[ 1 ], p[ 2 ] );

		if ( Input::Manager::instance().keyboard().isOn( 'z' ) ){
			gZMove -= 1.f;
		}else if ( Input::Manager::instance().keyboard().isOn( 'w' ) ){
			gZMove += 1.f; 
		}
		if ( Input::Manager::instance().keyboard().isOn( 'a' ) ){
			gXMove -= 10.f;
		}else if ( Input::Manager::instance().keyboard().isOn( 's' ) ){
			gXMove += 10.f; 
		}
		//表示
		ostringstream oss;
		oss << "[w-z] Z MOVEMENT: " << gZMove;
		drawDebugString( 10, 0, oss.str().c_str() );
		oss.str( "" );
		oss << "[a-s] X MOVEMENT: " << gXMove;
		drawDebugString( 10, 1, oss.str().c_str() );
	}
}
