#ifndef INCLUDED_SEQUENCE_GAME_PAUSE_H
#define INCLUDED_SEQUENCE_GAME_PAUSE_H

#include "Sequence/Game/Child.h"
class Image;

namespace Sequence{
namespace Game{
class Parent;

class Pause : public Child{
public:
	Pause();
	~Pause();
	Child* update( Parent* );
private:
	Image* mImage;
	int mCursorPosistion; 
};

} //namespace Game
} //namespace Sequence

#endif