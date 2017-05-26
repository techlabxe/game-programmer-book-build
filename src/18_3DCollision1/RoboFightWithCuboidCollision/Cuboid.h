#ifndef INCLUDED_CUBOID_H
#define INCLUDED_CUBOID_H

class Vector3;

class Cuboid{
public:
	bool isIntersect( const Cuboid& ) const;

	void setPosition( const Vector3& );
	void setHalfSize( const Vector3& );
	//受け取った移動ベクタの成分のうち相手に当たらないで動ける成分だけを残す
	void restrictMove( Vector3* moveVectorInOut, const Cuboid& ) const;
private:
	int mX;
	int mY;
	int mZ;
	int mHalfSizeX;
	int mHalfSizeY;
	int mHalfSizeZ;
};

#endif

