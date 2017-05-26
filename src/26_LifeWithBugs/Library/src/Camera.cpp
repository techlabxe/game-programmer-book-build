#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Matrix34.h"
using namespace GameLib;
#include "Library/Camera.h"


Camera::Camera() :
mPosition( 0.0, 0.0, 1.0 ),
mTarget( 0.0, 0.0, 0.0 ),
mFieldOfViewY( 60.0 ),
mNearClip( 1.0 ),
mFarClip( 10000.0 ){
}

Camera::~Camera(){
}

void Camera::setPosition( const Vector3& a ){
	mPosition = a;
}

void Camera::setTarget( const Vector3& a ){
	mTarget = a;
}

void Camera::setFieldOfViewY( float a ){
	mFieldOfViewY = a;
}

void Camera::setNearClip( float a ){
	mNearClip = a;
}

void Camera::setFarClip( float a ){
	mFarClip = a;
}

const Vector3* Camera::getPosition() const {
	return &mPosition;
}

const Vector3* Camera::getTarget() const {
	return &mTarget;
}

float Camera::getFieldOfViewY() const {
	return mFieldOfViewY;
}

float Camera::getNearClip() const {
	return mNearClip;
}

float Camera::getFarClip() const {
	return mFarClip;
}

void Camera::createPerspectiveViewMatrix( Matrix44* m ) const {
	Framework f = Framework::instance();
	m->setPerspectiveTransform(
		mFieldOfViewY, 
		static_cast< float >( f.width() ),
		static_cast< float >( f.height() ),
		mNearClip,
		mFarClip );
	Matrix34 vm;
	vm.setViewTransform( mPosition, mTarget, Vector3( 0.f, 1.f, 0.f ) );
	*m *= vm;
}
