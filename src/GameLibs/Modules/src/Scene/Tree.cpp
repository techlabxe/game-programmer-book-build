#include "GameLib/GameLib.h"
#include "GameLib/Scene/Tree.h"
#include "GameLib/Scene/Batch.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Base/Impl/ReferenceType.h"
#include "GameLib/Graphics/Manager.h"
#include "GameLib/Graphics/Enum.h"

#include "Scene/TreeImpl.h"

namespace GameLib{
using namespace GameLib::Graphics;
namespace Scene{

Tree::Tree( Impl* impl ) : mImpl( impl ){
	if ( mImpl ){
		mImpl->refer();
	}
}

void Tree::draw() const {
	mImpl->draw();
}

void Tree::setAnimation( Animation a ){
	mImpl->setAnimation( a.mImpl );
}

void Tree::updateAnimation( float time ){
	mImpl->updateAnimation( time );
}

void Tree::setAnimationSpeed( float s ){
	mImpl->setAnimationSpeed( s );
}

const Vector3* Tree::position() const {
	return &mImpl->mPosition;
}

const Vector3* Tree::angle() const {
	return &mImpl->mAngle;
}

const Vector3* Tree::scale() const {
	return &mImpl->mScale;
}

const Vector3* Tree::color() const {
	return &mImpl->mColor;
}

float Tree::transparency() const {
	return mImpl->mTransparency;
}

void Tree::setPosition( const Vector3& a ){
	mImpl->mPosition = a;
}

void Tree::setAngle( const Vector3& a ){
	mImpl->mAngle = a;
}

void Tree::setScale( const Vector3& a ){
	mImpl->mScale = a;
}

void Tree::setColor( const Vector3& a ){
	mImpl->mColor = a;
}

void Tree::setTransparency( float a ){
	mImpl->mTransparency = a;
}

#define TYPE Tree
#include "GameLib/Base/Impl/ReferenceTypeTemplate.h"
#define TYPE Tree
#define CONST_TYPE ConstTree
#include "GameLib/Base/Impl/ConstantTypeTemplate.h"

void ConstTree::draw() const {
	mBase.draw();
}

const Vector3* ConstTree::position() const {
	return mBase.position();
}

const Vector3* ConstTree::angle() const {
	return mBase.angle();
}

const Vector3* ConstTree::scale() const {
	return mBase.scale();
}

const Vector3* ConstTree::color() const {
	return mBase.color();
}

float ConstTree::transparency() const {
	return mBase.transparency();
}

} //namespace Scene
} //namespace GameLib