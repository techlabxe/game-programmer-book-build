#ifndef INCLUDED_SEQUENCE_GAME_PLAY_H
#define INCLUDED_SEQUENCE_GAME_PLAY_H

#include "Sequence/Game/Child.h"

namespace Sequence{
namespace Game{
class Parent;

class Play : public Child{
public:
	Play();
	~Play();
	void update( Parent* );
};

} //namespace Game
} //namespace Sequence

#endif