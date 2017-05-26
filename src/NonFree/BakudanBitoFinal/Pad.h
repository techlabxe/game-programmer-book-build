#ifndef INCLUDED_PAD_H
#define INCLUDED_PAD_H

class Pad{
public:
	enum Button{
		A, //0ボタンか'd'
		B, //1ボタンか'x'
		U, //up
		D, //down
		L, //left
		R, //right
		Q, //quit。終了ボタン
	};
	//なんでもいいからボタン
	static bool isOn( Button, int playerID = 0 );
	static bool isTriggered( Button, int playerID = 0 );
};

#endif