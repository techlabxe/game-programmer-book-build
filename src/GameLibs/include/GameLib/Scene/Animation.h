#ifndef INCLUDED_GAMELIB_SCENE_ANIMATION_H
#define INCLUDED_GAMELIB_SCENE_ANIMATION_H

namespace GameLib{
namespace Scene{

///アニメーションデータ
class Animation{
public:
	Animation();
	~Animation();
	Animation( const Animation& );
	operator void*() const;
	Animation& operator=( const Animation& );
	void release();
	bool operator==( const Animation& ) const;
	bool operator!=( const Animation& ) const;

	class Impl;
	explicit Animation( Impl* );
private:
	friend class Tree;
	Impl* mImpl;
};

} //namespace Scene
} //namespace GameLib

#endif
