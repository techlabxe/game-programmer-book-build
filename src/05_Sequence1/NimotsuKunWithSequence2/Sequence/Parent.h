#ifndef INCLUDED_SEQUENCE_PARENT_H
#define INCLUDED_SEQUENCE_PARENT_H

namespace Sequence{

class Title;
class StageSelect;
namespace Game{
	class Parent;
}

class Parent{
public:
	enum SeqID{
		SEQ_STAGE_SELECT,
		SEQ_TITLE,
		SEQ_GAME,

		SEQ_NONE,
	};
	Parent();
	~Parent();
	void update();

	void moveTo( SeqID );
	void setStageID( int stageID );
private:
	Title* mTitle;
	StageSelect* mStageSelect;
	Game::Parent* mGame;

	SeqID mNext;
	int mStageID;
};

} //namespace Sequence

#endif