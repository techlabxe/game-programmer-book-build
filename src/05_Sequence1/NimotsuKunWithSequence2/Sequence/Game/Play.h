#ifndef INCLUDED_SEQUENCE_GAME_PLAY_H
#define INCLUDED_SEQUENCE_GAME_PLAY_H

namespace Sequence{
namespace Game{
class Parent;

class Play{
public:
	Play();
	~Play();
	void update( Parent* );
};

} //namespace Game
} //namespace Sequence

#endif