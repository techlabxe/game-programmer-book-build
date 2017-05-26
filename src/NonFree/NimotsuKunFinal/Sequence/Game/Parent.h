#ifndef INCLUDED_SEQUENCE_GAME_H
#define INCLUDED_SEQUENCE_GAME_H

class State;

namespace Sequence{
class Parent;

namespace Game{

class Clear;
class Loading;
class Menu;
class Play;

class Parent{
public:
	typedef Sequence::Parent GrandParent; //ëcïÉ
	enum SeqID{
		SEQ_STAGE_SELECT, //è„äKëw
		SEQ_TITLE, //è„äKëw
		SEQ_CLEAR,
		SEQ_MENU,
		SEQ_PLAY,

		SEQ_NONE,
	};
	void moveTo( SeqID );
	Parent( int stageID );
	~Parent();
	void update( GrandParent* );
	void startLoading();
	State* getState();
private:
	State* mState;
	int mStageID;
	SeqID mNext;

	Clear* mClear;
	Loading* mLoading;
	Menu* mMenu;
	Play* mPlay;
};

} //namepsace Game
} //namespace Sequence

#endif