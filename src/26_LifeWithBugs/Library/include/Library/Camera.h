#ifndef INCLUDED_CAMERA_H
#define INCLUDED_CAMERA_H

#include "GameLib/Math/Vector3.h"
namespace GameLib{
	namespace Math{
		class Matrix44;
	}
}
using namespace GameLib::Math;

class Camera{
public:
	Camera();
	~Camera();
	void setPosition( const Vector3& );
	void setTarget( const Vector3& );
	void setFieldOfViewY( float );
	void setNearClip( float );
	void setFarClip( float );

	const Vector3* getPosition() const;
	const Vector3* getTarget() const;
	float getFieldOfViewY() const;
	float getNearClip() const;
	float getFarClip() const;

	void createPerspectiveViewMatrix( Matrix44* ) const;
private:
	Vector3 mPosition;
	Vector3 mTarget;
	float mFieldOfViewY;
	float mNearClip;
	float mFarClip;
};


#endif