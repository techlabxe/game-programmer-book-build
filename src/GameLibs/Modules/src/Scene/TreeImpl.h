#ifndef INCLUDED_GAMELIB_SCENE_TREEIMPL_H
#define INCLUDED_GAMELIB_SCENE_TREEIMPL_H

#include "GameLib/Base/Impl/ReferenceType.h"
#include "GameLib/Scene/Batch.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Base/Impl/ReferenceType.h"
#include "GameLib/Base/Pool.h"
#include "GameLib/Graphics/Manager.h"
#include "GameLib/Scene/Animation.h"

#include "Scene/BatchImpl.h"
#include "Scene/Node.h"
#include "Scene/TreeTemplate.h"
#include "Scene/ContainerImpl.h"
#include "Scene/AnimationImpl.h"

namespace GameLib{
using namespace GameLib::Graphics;
namespace Scene{

class Tree::Impl : public ReferenceType{
public:
	Impl( TreeTemplate* tmpl ) : 
	mTemplate( tmpl ),
	mPosition( 0.f, 0.f, 0.f ),
	mAngle( 0.f, 0.f, 0.f ),
	mScale( 1.f, 1.f, 1.f ),
	mColor( 1.f, 1.f, 1.f ),
	mTransparency( 1.f ),
	mTime( 0.f ),
	mSpeed( 1.f ),
	mNodes( 0 ),
	mNodeNumber( 0 ){
		mTemplate->container()->refer(); //所有コンテナの参照カウントを増やす

		//構築開始
		mNodeNumber = tmpl->nodeNumber();
		mNodes = static_cast< Node* >( OPERATOR_NEW( sizeof( Node ) * mNodeNumber ) );
		for ( int i = 0; i < mNodeNumber; ++i ){
			const NodeTemplate* src = tmpl->node( i ); 
			Node* dst = &mNodes[ i ];
			//コンストラクト
			new ( dst ) Node( src );

			//子の数を数える
			int child = src->mChild;
			int childNumber = 0;
			while ( child >= 0 ){
				++childNumber;
				child = tmpl->node( child )->mBrother;
			}
			Node** children = mChildrenPool.allocate( childNumber );
			//子を充填する
			child = src->mChild;
			int j = 0;
			while ( child >= 0 ){
				children[ j ] = mNodes + child;
				child = tmpl->node( child )->mBrother;
				++j;
			}
			//ノードに与える
			dst->setChildren( children, childNumber );
		}
	}
	~Impl(){
		for ( int i = 0; i < mNodeNumber; ++i ){
			mNodes[ i ].~Node();
		}
		OPERATOR_DELETE( mNodes );

		//親コンテナの解放チェック
		Container::Impl* c = mTemplate->container(); 
		c->release();
		if ( c->referenceCount() == 0 ){ //自分が最後の参照を持っている。破棄。
			SAFE_DELETE( c );
		}
		mTemplate = 0;
	}
	void draw(){
		//ワールド行列を作る
		Matrix34 w;
		w.setTranslation( mPosition );
		w.rotateY( mAngle.y );
		w.rotateX( mAngle.x );
		w.rotateZ( mAngle.z );
		w.scale( mScale );
		//描画
		mNodes[ 0 ].draw( w, mColor, mTransparency );
	}
	void setAnimation( Animation::Impl* a ){
		if ( a ){ //モノがあれば
			for ( int i = 0; i < mNodeNumber; ++i ){
				mNodes[ i ].setAnimation( a->node( mNodes[ i ].name() ) );
			}
		}else{ //モノがなければ
			for ( int i = 0; i < mNodeNumber; ++i ){
				mNodes[ i ].removeAnimation(); //状態も初期化
			}
		}
		mTime = 0.f;
	}
	void setAnimationSpeed( float s ){
		mSpeed = s;
	}
	void updateAnimation( float t ){
		if ( t >= 0.f ){
			mTime = t;
		}
		for ( int i = 0; i < mNodeNumber; ++i ){
			mNodes[ i ].updateAnimation( mTime );
		}
		mTime += mSpeed;
	}
	TreeTemplate* mTemplate;
	Vector3 mPosition;
	Vector3 mAngle;
	Vector3 mScale;
	Vector3 mColor;
	float mTransparency;
	float mTime; //アニメ時刻
	float mSpeed; //アニメ速度

	Node* mNodes; //0番が根と決まっている
	int mNodeNumber;

	Pool< Node* > mChildrenPool; //各ノードが持つ子配列をここに集中
};

} //namespace Scene
} //namespace GameLib

#endif