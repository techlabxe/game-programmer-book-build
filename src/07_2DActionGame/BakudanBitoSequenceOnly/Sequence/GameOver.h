#ifndef INCLUDED_SEQUENCE_GAME_OVER_H
#define INCLUDED_SEQUENCE_GAME_OVER_H

class Image;

namespace Sequence{
class Parent;

class GameOver{
public:
	GameOver();
	~GameOver();
	void update( Parent* );
private:
	Image* mImage;
	int mCount;
};

} //namespace Sequence

#endif
