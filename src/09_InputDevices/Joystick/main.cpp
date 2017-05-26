#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Joystick.h"
#include <sstream>
using namespace std;

namespace GameLib{
	void Framework::update(){
		using namespace Input;
		int joystickNumber = Manager::instance().joystickNumber();
		for ( int i = 0; i < joystickNumber; ++i ){
			Joystick joystick = Input::Manager::instance().joystick( i );
			if ( !joystick.isEnabled() ){
				drawDebugString( 0, 0 + i*10, "not enabled!" );
				continue;
			}

			ostringstream oss;

			oss << "ANALOGS: ";
			int analogNumber = joystick.analogNumber();
			for ( int j = 0; j < analogNumber; ++j ){
				oss << joystick.analog( j ) << " ";
			}
			drawDebugString( 0, 0 + i*10, oss.str().c_str() );
			oss.str( "" );

			oss << "BUTTONS: ";
			int buttonNumber = joystick.buttonNumber();
			for ( int j = 0; j < buttonNumber; ++j ){
				oss << joystick.isOn( j ) << " ";
			}
			drawDebugString( 0, 1 + i*10, oss.str().c_str() );
			oss.str( "" );

			bool left, right, down, up;
			left = joystick.isOn( Joystick::BUTTON_LEFT );
			right = joystick.isOn( Joystick::BUTTON_RIGHT );
			down = joystick.isOn( Joystick::BUTTON_DOWN );
			up = joystick.isOn( Joystick::BUTTON_UP );
			oss << "DIRECTION:";
			if ( left ){
				oss << " LEFT";
			}
			if ( right ){
				oss << " RIGHT";
			}
			if ( up ){
				oss << " UP";
			}
			if ( down ){
				oss << " DOWN";
			}
			drawDebugString( 0, 2 + i*10, oss.str().c_str() );
		}
	}
}