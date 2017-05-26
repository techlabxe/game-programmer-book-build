#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;
#include "Library/Camera.h"
#include "Library/Matrix34.h"
#include "Library/Matrix44.h"

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

void Camera::setFieldOfViewY( double a ){
	mFieldOfViewY = a;
}

void Camera::setNearClip( double a ){
	mNearClip = a;
}

void Camera::setFarClip( double a ){
	mFarClip = a;
}

const Vector3* Camera::position() const {
	return &mPosition;
}

const Vector3* Camera::target() const {
	return &mTarget;
}

double Camera::fieldOfViewY() const {
	return mFieldOfViewY;
}

double Camera::nearClip() const {
	return mNearClip;
}

double Camera::farClip() const {
	return mFarClip;
}

void Camera::createPerspectiveViewMatrix( Matrix44* m ) const {
	Framework f = Framework::instance();
	m->setPerspectiveTransform(
		mFieldOfViewY,
		static_cast< double >( f.width() ),
		static_cast< double >( f.height() ),
		mNearClip,
		mFarClip );
	Matrix34 vm;
	vm.setViewTransform( mPosition, mTarget );
	*m *= vm;
}
