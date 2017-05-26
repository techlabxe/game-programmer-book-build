#ifndef INCLUDED_SEQUENCE_GAME_OVER_H
#define INCLUDED_SEQUENCE_GAME_OVER_H

#include "Sequence/Child.h"

class Image;

namespace Sequence{
class Parent;

class GameOver : public Child{
public:
	GameOver();
	~GameOver();
	Child* update( Parent* );
private:
	Image* mImage;
	int mCount;
};

} //namespace Sequence

#endif
