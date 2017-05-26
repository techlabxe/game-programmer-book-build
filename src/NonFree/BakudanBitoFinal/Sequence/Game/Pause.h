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
	void update( Parent* );
private:
	Image* mImage;
	Image* mCursorImage;
	int mCursorPosition; 
};

} //namespace Game
} //namespace Sequence

#endif