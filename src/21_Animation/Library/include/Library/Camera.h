#ifndef INCLUDED_CAMERA_H
#define INCLUDED_CAMERA_H

#include "Library/Vector3.h"
class Matrix44;

class Camera{
public:
	Camera();
	~Camera();
	void setPosition( const Vector3& );
	void setTarget( const Vector3& );
	void setFieldOfViewY( double );
	void setNearClip( double );
	void setFarClip( double );

	const Vector3* position() const;
	const Vector3* target() const;
	double fieldOfViewY() const;
	double nearClip() const;
	double farClip() const;

	void createPerspectiveViewMatrix( Matrix44* ) const;
private:
	Vector3 mPosition;
	Vector3 mTarget;
	double mFieldOfViewY;
	double mNearClip;
	double mFarClip;
};


#endif