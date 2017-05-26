#ifndef INCLUDED_SEQUENCE_GAME_LOADING_H
#define INCLUDED_SEQUENCE_GAME_LOADING_H

class Image;

namespace Sequence{
namespace Game{
class Parent;

class Loading{
public:
	Loading();
	~Loading();
	void update( Parent* );
private:
	Image* mImage;
	int mCount;
	bool mStarted; //ロード開始要請を出したか
};

} //namespace Game
} //namespace Sequence

#endif