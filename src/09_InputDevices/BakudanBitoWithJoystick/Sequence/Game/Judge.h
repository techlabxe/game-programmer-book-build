#ifndef INCLUDED_SEQUENCE_GAME_JUDGE_H
#define INCLUDED_SEQUENCE_GAME_JUDGE_H

class Image;

namespace Sequence{
namespace Game{
class Parent;

class Judge{
public:
	Judge();
	~Judge();
	void update( Parent* );
private:
	Image* mImage;
	int mCursorPosistion; 
};

} //namespace Game
} //namespace Sequence

#endif
