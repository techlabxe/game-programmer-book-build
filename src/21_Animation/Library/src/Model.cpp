#include "GameLib/GameLib.h"
#include "Library/Model.h"
#include "Library/Batch.h"
#include "Library/Matrix44.h"
#include "Library/Matrix34.h"

Model::Model( const Batch* batch ) :
mPosition( 0.0, 0.0, 0.0 ),
mAngle( 0.0, 0.0, 0.0 ),
mScale( 1.0, 1.0, 1.0 ),
mColor( 1.0, 1.0, 1.0 ),
mBatch( batch ){
}

Model::~Model(){
	mBatch = 0;
}

void Model::draw( 
const Matrix44& pvm,
const Vector3& lightVector,
const Vector3& lightColor,
const Vector3& ambient ) const {
	Matrix34 wm;
	wm.setTranslation( mPosition );
	wm.rotateY( mAngle.y );
	wm.rotateX( mAngle.x );
	wm.rotateZ( mAngle.z );
	wm.scale( mScale );

	mBatch->draw( 
		pvm,
		wm,
		lightVector,
		lightColor,
		ambient,
		mColor );
}

void Model::setPosition( const Vector3& a ){
	mPosition = a;
}

void Model::setAngle( const Vector3& a ){
	mAngle = a;
}

void Model::setScale( const Vector3& a ){
	mScale = a;
}

void Model::setColor( const Vector3& c ){
	mColor = c;
}

const Batch* Model::batch() const {
	return mBatch;
}

const Vector3* Model::position() const {
	return &mPosition;
}

const Vector3* Model::angle() const {
	return &mAngle;
}

const Vector3* Model::scale() const {
	return &mScale;
}
