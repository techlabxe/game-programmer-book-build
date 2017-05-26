#ifndef INCLUDED_SEQUENCE_GAME_CLEAR_H
#define INCLUDED_SEQUENCE_GAME_CLEAR_H

#include "Sequence/Game/Child.h"
class Image;

namespace Sequence{
namespace Game{
class Parent;

class Clear : public Child{
public:
	Clear();
	~Clear();
	Child* update( Parent* );
private:
	Image* mImage;
	int mCount;
};

} //namespace Game
} //namespace Sequence

#endif