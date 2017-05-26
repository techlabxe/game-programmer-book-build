#ifndef INCLUDED_GAMELIB_SCENE_NODE_H
#define INCLUDED_GAMELIB_SCENE_NODE_H

#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Functions.h"
#include "Scene/NodeTemplate.h"
#include "Scene/AnimationNode.h"

namespace GameLib{
namespace Scene{
using namespace GameLib::Math;

class Node{
public:
	Node( const NodeTemplate* tmpl ) : 
	mTranslation( 0.f, 0.f, 0.f ),
	mRotation( 0.f, 0.f, 0.f ),
	mScale( 1.f, 1.f, 1.f ),
	mChildren( 0 ),
	mChildNumber( 0 ),
	mTemplate( tmpl ),
	mAnimation( 0 ){
	}
	~Node(){
		mTemplate = 0; //単なる参照
		mAnimation = 0;
	}
	void setChildren( Node** children, int childNumber ){
		mChildren = children;
		mChildNumber = childNumber;
	}
	void draw( const Matrix34& parentMatrix, const Vector3& color, float transparency ) const {
		//ワールド行列を作る
		Matrix34 w = parentMatrix;
		w.setMul( parentMatrix, mTemplate->mTransform ); //デフォルトをかけて、
		w.translate( mTranslation );
		w.rotateY( mRotation.y );
		w.rotateX( mRotation.x );
		w.rotateZ( mRotation.z );
		w.scale( mScale );

		if ( mTemplate->mBatch ){
			//セット
			Graphics::Manager().setWorldMatrix( w );
			//描画
			mTemplate->mBatch->draw( color, transparency );
		}
		for ( int i = 0; i < mChildNumber; ++i ){
			mChildren[ i ]->draw( w, color, transparency );
		}
	}
	void setAnimation( const AnimationNode* a ){
		mAnimation = a;
	}
	void removeAnimation(){
		mAnimation = 0;
		mTranslation.set( 0.f, 0.f, 0.f );
		mRotation.set( 0.f, 0.f, 0.f );
		mScale.set( 1.f, 1.f, 1.f );
	}
	void updateAnimation( float t ){
		if ( mAnimation ){
			mAnimation->update(
				&mTranslation,
				&mRotation,
				&mScale,
				t );
		}
	}
	const char* name() const {
		return mTemplate->mName;
	}
private:
	Vector3 mTranslation;
	Vector3 mRotation;
	Vector3 mScale;
	Node** mChildren;
	int mChildNumber;
	const NodeTemplate* mTemplate;
	const AnimationNode* mAnimation;
};

} //namespace Scene
} //namespace GameLib

#endif

