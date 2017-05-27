#ifndef INCLUDED_GAMELIB_INPUT_JOYSTICK_H
#define INCLUDED_GAMELIB_INPUT_JOYSTICK_H

namespace GameLib{
namespace Input{

class Joystick{
public:
	enum Button{
		BUTTON_UP = 128,
		BUTTON_DOWN,
		BUTTON_LEFT,
		BUTTON_RIGHT,
	};

	int analogNumber() const;
	int analog( int ) const; //アナログスティック値取得
	//上下左右は上の列挙で取れます。ほかのボタンは0からの数字で。
	bool isOn( int ) const;
	bool isTriggered( int ) const;
	int buttonNumber() const;
	//有効か？
	bool isEnabled() const;

	Joystick();
	~Joystick();
	operator void*() const;
	class Impl;
private:
	friend class Manager;
	Joystick( Impl* );
	Impl* mImpl;
};

} //namespace Input
} //namespace GameLib

#endif
