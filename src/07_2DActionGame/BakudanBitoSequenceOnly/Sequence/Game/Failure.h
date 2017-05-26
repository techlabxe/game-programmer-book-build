#ifndef INCLUDED_SEQUENCE_GAME_FAILURE_H
#define INCLUDED_SEQUENCE_GAME_FAILURE_H

class Image;

namespace Sequence{
namespace Game{
class Parent;

class Failure{
public:
	Failure();
	~Failure();
	void update( Parent* );
private:
	Image* mImage;
	int mCount;
};

} //namespace Game
} //namespace Sequence

#endif