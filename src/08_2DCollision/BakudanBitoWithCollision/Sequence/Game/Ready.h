#ifndef INCLUDED_SEQUENCE_GAME_READY_H
#define INCLUDED_SEQUENCE_GAME_READY_H

class Image;

namespace Sequence{
namespace Game{
class Parent;

class Ready{
public:
	Ready();
	~Ready();
	void update( Parent* );
private:
	Image* mImage;
	int mCount;
	bool mStarted;
};

} //namespace Game
} //namespace Sequence

#endif