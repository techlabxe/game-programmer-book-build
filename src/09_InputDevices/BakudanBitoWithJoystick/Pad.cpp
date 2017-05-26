#include "GameLib/GameLib.h"
#include "Pad.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Joystick.h"
using namespace GameLib;

bool Pad::isOn( Button b, int pid ){
	Input::Manager im = Input::Manager::instance();
	Input::Keyboard kb = im.keyboard();
	bool ret = false;
	if ( pid == 0 ){
		int table[] = { 'd', 'x', 'w', 'z', 'a', 's', 'q' };
		ret = kb.isOn( table[ b ] );
	}else{
		int table[] = { 'l', ',', 'i', 'm', 'j', 'k', 'q' };
		ret = kb.isOn( table[ b ] );
	}
	if ( im.joystickNumber() > pid ){
		Input::Joystick j = im.joystick( pid );
		int table[] = {
			0,
			1,
			Input::Joystick::BUTTON_UP,
			Input::Joystick::BUTTON_DOWN,
			Input::Joystick::BUTTON_LEFT,
			Input::Joystick::BUTTON_RIGHT,
			2,
		};
		if ( j.isOn( table[ b ] ) ){
			ret = true;
		}
	}
	return ret;
}

bool Pad::isTriggered( Button b, int pid ){
	Input::Manager im = Input::Manager::instance();
	Input::Keyboard kb = im.keyboard();
	bool ret = false;
	if ( pid == 0 ){
		int table[] = { 'd', 'x', 'w', 'z', 'a', 's', 'q' };
		ret = kb.isTriggered( table[ b ] );
	}else{
		int table[] = { 'l', ',', 'i', 'm', 'j', 'k', 'q' };
		ret = kb.isTriggered( table[ b ] );
	}
	if ( im.joystickNumber() > pid ){
		Input::Joystick j = im.joystick( pid );
		int table[] = {
			0,
			1,
			Input::Joystick::BUTTON_UP,
			Input::Joystick::BUTTON_DOWN,
			Input::Joystick::BUTTON_LEFT,
			Input::Joystick::BUTTON_RIGHT,
			2,
		};
		if ( j.isTriggered( table[ b ] ) ){
			ret = true;
		}
	}
	return ret;
}
