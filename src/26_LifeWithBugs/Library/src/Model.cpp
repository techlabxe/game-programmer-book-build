#include "GameLib/GameLib.h"
#include "Library/Model.h"
#include "Library/Batch.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Graphics/Manager.h"

Model::Model( ConstSharedPtr< Batch > batch ) :
mPosition( 0.0, 0.0, 0.0 ),
mAngle( 0.0, 0.0, 0.0 ),
mScale( 1.0, 1.0, 1.0 ),
mBatch( batch ){
}

Model::~Model(){
}

void Model::draw() const {
	Matrix34 wm;
	wm.setTranslation( mPosition );
	wm.rotateY( mAngle.y );
	wm.rotateX( mAngle.x );
	wm.rotateZ( mAngle.z );
	wm.scale( mScale );

	GameLib::Graphics::Manager::instance().setWorldMatrix( wm );
	mBatch->draw();
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

ConstSharedPtr< Batch > Model::getBatch() const {
	return mBatch;
}

const Vector3* Model::getPosition() const {
	return &mPosition;
}

const Vector3* Model::getAngle() const {
	return &mAngle;
}
