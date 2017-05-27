#ifndef INCLUDED_GAMELIB_SCENE_TREE_H
#define INCLUDED_GAMELIB_SCENE_TREE_H

namespace GameLib{

namespace Math{
	class Matrix44;
	class Vector3;
	class Vector4;
};

namespace Scene{
class Animation;
using namespace Math;

///階層構造モデル
class Tree{
public:
	///描画
	void draw() const;
	///アニメーション設定
	void setAnimation( Animation );
	///アニメーション更新(引数は時刻。マイナスなら中で勝手に進める)
	void updateAnimation( float time = -1.f );
	///アニメ速度設定
	void setAnimationSpeed( float );

	const Vector3* position() const;
	const Vector3* angle() const; //単位は度
	const Vector3* scale() const;
	const Vector3* color() const;
	float transparency() const;
	void setPosition( const Vector3& );
	void setAngle( const Vector3& ); //単位は度
	void setScale( const Vector3& );
	void setColor( const Vector3& );
	void setTransparency( float );

	Tree();
	~Tree();
	Tree( const Tree& );
	operator void*() const;
	Tree& operator=( const Tree& );
	void release();
	bool operator==( const Tree& ) const;
	bool operator!=( const Tree& ) const;

	class Impl;
	explicit Tree( Impl* );
private:
	Impl* mImpl;
};

//const版
class ConstTree{
public:
	void draw() const;
	const Vector3* position() const;
	const Vector3* angle() const;
	const Vector3* scale() const;
	const Vector3* color() const;
	float transparency() const;

	//以下知らなくていい
	ConstTree();
	ConstTree( const Tree& );
	~ConstTree();
	operator void*() const;
	ConstTree& operator=( const Tree& );
	void release();
	bool operator==( const Tree& ) const;
	bool operator!=( const Tree& ) const;
private:
	Tree mBase;
};


} //namespace Scene
} //namespace GameLib

#endif
