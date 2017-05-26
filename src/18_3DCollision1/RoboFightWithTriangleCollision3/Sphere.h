#ifndef INCLUDED_SPHERE_H
#define INCLUDED_SPHERE_H

#include "Library/Vector3.h"

class Sphere{
public:
	bool isIntersect( const Sphere& ) const;

	void setPosition( const Vector3& );
	void setRadius( double );
	//受け取った移動ベクタの成分のうち相手に当たらないで動ける成分だけを残す
	void restrictMove( Vector3* moveVectorInOut, const Sphere& ) const;
private:
	Vector3 mPosition;
	double mRadius;
};

#endif

