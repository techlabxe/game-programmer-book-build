#ifndef INCLUDED_GAMELIB_INPUT_MOUSE_H
#define INCLUDED_GAMELIB_INPUT_MOUSE_H

namespace GameLib{
namespace Input{

class Mouse{
public:
	enum Button{
		BUTTON_LEFT,
		BUTTON_RIGHT,
		BUTTON_MIDDLE,
	};
	int x() const;
	int y() const;
	int velocityX() const;
	int velocityY() const;
	int wheel() const;

	bool isOn( Button ) const;
	bool isTriggered( Button ) const;
};

} //namespace Input
} //namespace GameLib

#endif
