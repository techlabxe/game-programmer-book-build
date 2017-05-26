#include "GameLib/GameLib.h"
#include "Library/Node.h"
#include "Library/TreeTemplate.h"
#include "Library/NodeTemplate.h"
#include "Library/AnimationNode.h"
#include "Library/Batch.h"
#include "Library/Matrix44.h"
#include "Library/Matrix34.h"

Node::Node() :
mTranslation( 0.0, 0.0, 0.0 ),
mRotation( 0.0, 0.0, 0.0 ),
mScale( 1.0, 1.0, 1.0 ),
mColor( 1.0, 1.0, 1.0 ),
mBatch( 0 ),
mChildren( 0 ),
mChildNumber( 0 ){
}

Node::Node( const Batch* batch, int childNumber ) :
mTranslation( 0.0, 0.0, 0.0 ),
mRotation( 0.0, 0.0, 0.0 ),
mScale( 1.0, 1.0, 1.0 ),
mColor( 1.0, 1.0, 1.0 ),
mBatch( batch ),
mChildren( 0 ),
mChildNumber( childNumber ),
mAnimation( 0 ){
	mChildren = new Node*[ mChildNumber ];
	//îOÇÃÇΩÇﬂ0ñÑÇﬂÅBéÄÇÒÇ≈Ç≠ÇÍÇÈÇæÇÎÇ§
	for ( int i = 0; i < mChildNumber; ++i ){
		mChildren[ i ] = 0;
	}
}

Node::~Node(){
	mBatch = 0;
	mAnimation = 0;
	SAFE_DELETE_ARRAY( mChildren );
}

void Node::draw( 
const Matrix44& pvm,
const Matrix34& parentWm,
const Vector3& lightVector,
const Vector3& lightColor,
const Vector3& ambient ) const {
	Matrix34 wm;
	wm.setTranslation( mTranslation );
	wm.rotateY( mRotation.y );
	wm.rotateX( mRotation.x );
	wm.rotateZ( mRotation.z );
	wm.scale( mScale );

	wm.setMul( parentWm, wm );

	if ( mBatch ){
		mBatch->draw( 
			pvm,
			wm,
			lightVector,
			lightColor,
			ambient,
			mColor );
	}
	//éqÇ÷éÛÇØåpÇÆ
	for ( int i = 0; i < mChildNumber; ++i ){
		ASSERT( mChildren[ i ] ); //0Ç™ì¸Ç¡ÇƒÇΩÇÁàŸèÌÅB
		mChildren[ i ]->draw(
			pvm,
			wm,
			lightVector,
			lightColor,
			ambient );
	}
}

void Node::setTranslation( const Vector3& a ){
	mTranslation = a;
}

void Node::setRotation( const Vector3& a ){
	mRotation = a;
}

void Node::setScale( const Vector3& a ){
	mScale = a;
}

void Node::setColor( const Vector3& c ){
	mColor = c;
}

const Batch* Node::batch() const {
	return mBatch;
}

const Vector3* Node::translation() const {
	return &mTranslation;
}

const Vector3* Node::rotation() const {
	return &mRotation;
}

const Vector3* Node::scale() const {
	return &mScale;
}

void Node::setChild( int i, Node* n ){
	ASSERT( i >= 0 && i < mChildNumber );
	mChildren[ i ] = n;
}

void Node::setChildNumber( int n ){
	if ( mChildren ){
		SAFE_DELETE_ARRAY( mChildren );
	}
	mChildNumber = n;
	mChildren = new Node*[ mChildNumber ];
	//îOÇÃÇΩÇﬂ0ñÑÇﬂÅBéÄÇÒÇ≈Ç≠ÇÍÇÈÇæÇÎÇ§
	for ( int i = 0; i < mChildNumber; ++i ){
		mChildren[ i ] = 0;
	}
}

void Node::setBatch( const Batch* b ){
	mBatch = b;
}

const string* Node::name() const {
	return &mName;
}

void Node::setName( const char* name ){
	mName = name;
}

void Node::update( double time ){
	if ( mAnimation ){
		mAnimation->data( 
			&mTranslation,
			&mRotation,
			&mScale,
			time );
	}
}

void Node::setAnimation( const AnimationNode* an ){
	mAnimation = an;
}

