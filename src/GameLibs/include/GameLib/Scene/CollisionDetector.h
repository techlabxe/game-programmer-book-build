#ifndef INCLUDED_GAMELIB_COLLISIONDETECTOR_H
#define INCLUDED_GAMELIB_COLLISIONDETECTOR_H

namespace GameLib{
namespace Math{
	class Vector3;
};
template< class T > class Array;
namespace Scene{
using namespace Math;

///球衝突判定クラス
/*
毎フレームの使用を想定している。
addで登録、detectで計算を行って結果を返す。
addの効果はdetect一回分しかないので、
毎フレームaddすることに注意。

現状試作品。中はk-d treeなので見てみて欲しい。
*/
class CollisionDetector{
public:
	///結果の番号ペア
	struct Pair{
		int mId0, mId1;
	};
	static CollisionDetector create( int capacity, int maxLevel = -1 );
	///項目追加。detect一回だけ有効。戻り値は結果に入ってくるID。
	int add( const Vector3& sphereCenter, float sphereRadius );
	///検出関数。addしたものは消える。引数には結果IDペア配列を入れる。
	void detect( Array< Pair >* resultsOut );

	//以下気にしなくていい関数
	CollisionDetector();
	~CollisionDetector();
	CollisionDetector( const CollisionDetector& );
	operator void*() const;
	CollisionDetector& operator=( const CollisionDetector& );
	void release();
	bool operator==( const CollisionDetector& ) const;
	bool operator!=( const CollisionDetector& ) const;
private:
	class Impl;
	explicit CollisionDetector( Impl* );
	Impl* mImpl;
};

} //namespace Scene
} //namespace GameLib

#endif
