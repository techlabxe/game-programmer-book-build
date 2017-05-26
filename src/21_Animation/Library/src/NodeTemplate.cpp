#include "GameLib/GameLib.h"
#include "Library/NodeTemplate.h"

NodeTemplate::NodeTemplate() :
mChild( -1 ),
mBrother( -1 ), 
mBatch( 0 ),
mTranslation( 0.0, 0.0, 0.0 ),
mRotation( 0.0, 0.0, 0.0 ),
mScale( 1.0, 1.0, 1.0 ){
}

NodeTemplate::~NodeTemplate(){
}

int NodeTemplate::child() const {
	return mChild;
}

int NodeTemplate::brother() const {
	return mBrother;
}

const Batch* NodeTemplate::batch() const {
	return mBatch;
}

const Vector3* NodeTemplate::translation() const {
	return &mTranslation;
}

const Vector3* NodeTemplate::rotation() const {
	return &mRotation;
}

const Vector3* NodeTemplate::scale() const {
	return &mScale;
}

const string* NodeTemplate::name() const {
	return &mName;
}

