#include "GameLib/GameLib.h"
#include "GameLib/Scene/Model.h"
#include "GameLib/Scene/Batch.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Base/Impl/ReferenceType.h"
#include "GameLib/Graphics/Manager.h"
#include "GameLib/Graphics/Enum.h"

#include "Scene/ModelImpl.h"

namespace GameLib{
using namespace GameLib::Graphics;
namespace Scene{

Model::Model( Impl* impl ) : mImpl( impl ){
	if ( mImpl ){
		mImpl->refer();
	}
}

void Model::draw() const {
	mImpl->draw();
}

const Vector3* Model::position() const {
	return &mImpl->mPosition;
}

const Vector3* Model::angle() const {
	return &mImpl->mAngle;
}

const Vector3* Model::scale() const {
	return &mImpl->mScale;
}

const Vector3* Model::color() const {
	return &mImpl->mColor;
}

float Model::transparency() const {
	return mImpl->mTransparency;
}

void Model::setPosition( const Vector3& a ){
	mImpl->mPosition = a;
}

void Model::setAngle( const Vector3& a ){
	mImpl->mAngle = a;
}

void Model::setScale( const Vector3& a ){
	mImpl->mScale = a;
}

void Model::setColor( const Vector3& a ){
	mImpl->mColor = a;
}

void Model::setTransparency( float a ){
	mImpl->mTransparency = a;
}

#define TYPE Model
#include "GameLib/Base/Impl/ReferenceTypeTemplate.h"
#define TYPE Model
#define CONST_TYPE ConstModel
#include "GameLib/Base/Impl/ConstantTypeTemplate.h"

void ConstModel::draw() const {
	mBase.draw();
}

const Vector3* ConstModel::position() const {
	return mBase.position();
}

const Vector3* ConstModel::angle() const {
	return mBase.angle();
}

const Vector3* ConstModel::scale() const {
	return mBase.scale();
}

const Vector3* ConstModel::color() const {
	return mBase.color();
}

float ConstModel::transparency() const {
	return mBase.transparency();
}

} //namespace Scene
} //namespace GameLib