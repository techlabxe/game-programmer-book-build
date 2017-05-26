#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include <sstream>
using namespace std;

namespace GameLib{
	void Framework::update(){
		using namespace Input;
		sleep( 16 );
		Keyboard keyboard;
		keyboard = Manager::instance().keyboard();
		ostringstream oss;
		for ( int c = 0; c < 128; ++c ){
			if ( keyboard.isOn( c ) ){
				oss << static_cast< char >( c );
			}
		}
		drawDebugString( 0, 0, oss.str().c_str() );
		if ( keyboard.isOn( Keyboard::KEY_ALT ) ){
			drawDebugString( 0, 1, "ALT" );
		}
		if ( keyboard.isOn( Keyboard::KEY_RETURN ) ){
			drawDebugString( 0, 2, "RETURN" );
		}
		if ( keyboard.isOn( Keyboard::KEY_ESC ) ){
			drawDebugString( 0, 3, "ESC" );
		}
		if ( keyboard.isOn( Keyboard::KEY_CONTROL ) ){
			drawDebugString( 0, 4, "CONTROL" );
		}
		if ( keyboard.isOn( Keyboard::KEY_UP ) ){
			drawDebugString( 0, 5, "UP" );
		}
		if ( keyboard.isOn( Keyboard::KEY_DOWN ) ){
			drawDebugString( 0, 6, "DOWN" );
		}
		if ( keyboard.isOn( Keyboard::KEY_LEFT ) ){
			drawDebugString( 0, 7, "LEFT" );
		}
		if ( keyboard.isOn( Keyboard::KEY_RIGHT ) ){
			drawDebugString( 0, 8, "RIGHT" );
		}
		if ( keyboard.isOn( Keyboard::KEY_F1 ) ){
			drawDebugString( 0, 9, "F1" );
		}
		if ( keyboard.isOn( Keyboard::KEY_SHIFT ) ){
			drawDebugString( 0, 10, "SHIFT" );
		}
		if ( keyboard.isOn( ' ' ) ){
			drawDebugString( 0, 11, "SPACE" );
		}
		if ( keyboard.isOn( Keyboard::KEY_TAB ) ){
			drawDebugString( 0, 12, "TAB" );
		}
	}
}