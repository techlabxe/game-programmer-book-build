#ifndef INCLUDED_PAD_H
#define INCLUDED_PAD_H

class Pad{
public:
	enum Button{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		JUMP,
		FIRE,
		TURN,
	};
	static Pad* instance();
	static void create();
	static void destroy();
	bool isOn( Button, int id ) const;
	bool isTriggered( Button, int id ) const;
private:
	Pad();
	~Pad();
	static Pad* mInstance;
};

#endif

