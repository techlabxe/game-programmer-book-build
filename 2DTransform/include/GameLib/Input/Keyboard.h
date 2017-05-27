#ifndef INCLUDED_GAMELIB_INPUT_KEYBOARD_H
#define INCLUDED_GAMELIB_INPUT_KEYBOARD_H

namespace GameLib{
namespace Input{

class Keyboard{
public:
	enum Key{ //isKeyOn,isKeyPressedÇÃà¯êîÅBa-z,0-9ÇÕÇªÇÃÇ‹Ç‹ï∂éöíËêîÇ≈ì¸ÇÍÇÈÇ±Ç∆ÅB
		KEY_RETURN = 0x80, //128î‘à»ç~ÇégÇ§
		KEY_TAB,
		KEY_ESC,
		KEY_SHIFT,
		KEY_CONTROL,
		KEY_ALT,
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		KEY_UP,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_NUM0,
		KEY_NUM1,
		KEY_NUM2,
		KEY_NUM3,
		KEY_NUM4,
		KEY_NUM5,
		KEY_NUM6,
		KEY_NUM7,
		KEY_NUM8,
		KEY_NUM9,
	};
	bool isOn( int i ) const;
	bool isTriggered( int i ) const;
};

} //namespace Input
} //namespace GameLib


#endif
