#include "GameLib/GameLib.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"

#include "Library/Tree.h"
#include "Library/TreeTemplate.h"
#include "Library/NodeTemplate.h"
#include "Library/Node.h"
#include "Library/Animation.h"
#include "Library/AnimationNode.h"
#include "Library/Matrix44.h"
#include "Library/Matrix34.h"
#include "Library/Batch.h"

using namespace GameLib;
using namespace GameLib::PseudoXml;

Tree::Tree( const TreeTemplate& tmpl ) :
mPosition( 0.0, 0.0, 0.0 ),
mAngle( 0.0, 0.0, 0.0 ),
mScale( 1.0, 1.0, 1.0 ),
mNodes( 0 ),
mNodeNumber( 0 ),
mTime( 0.0 ){
	mNodeNumber = tmpl.nodeNumber();
	mNodes = new Node[ mNodeNumber ];
	for ( int i = 0; i < mNodeNumber; ++i ){
		Node& dst = mNodes[ i ];
		const NodeTemplate* src = tmpl.node( i );
		//パラメータを移す
		dst.setTranslation( *src->translation() );
		dst.setRotation( *src->rotation() );
		dst.setScale( *src->scale() );
		dst.setName( src->name()->c_str() );
		dst.setBatch( src->batch() );

		//[長男-兄弟形式から子配列形式への変換]
		//子の数を数える
		int child = src->child();
		int childNumber = 0;
		while ( child >= 0 ){
			++childNumber;
			child = tmpl.node( child )->brother();
		}
		//子をアロケート
		dst.setChildNumber( childNumber );
		//子を充填する
		child = src->child();
		int j = 0;
		while ( child >= 0 ){
			dst.setChild( j, mNodes + child );
			child = tmpl.node( child )->brother();
			++j;
		}
	}
}

Tree::~Tree(){
	SAFE_DELETE_ARRAY( mNodes );
}

void Tree::draw(
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

	//根ノードへ渡す
	if ( mNodes ){
		mNodes[ 0 ].draw(
			pvm,
			wm,
			lightVector,
			lightColor,
			ambient );
	}
}


void Tree::setPosition( const Vector3& a ){
	mPosition = a;
}

void Tree::setAngle( const Vector3& a ){
	mAngle = a;
}

void Tree::setScale( const Vector3& a ){
	mScale = a;
}

const Vector3* Tree::position() const {
	return &mPosition;
}

const Vector3* Tree::angle() const {
	return &mAngle;
}

const Vector3* Tree::scale() const {
	return &mScale;
}

Node* Tree::node( const char* name ){
	for ( int i = 0; i < mNodeNumber; ++i ){
		if ( *( mNodes[ i ].name() ) == name ){
			return &mNodes[ i ];
		}
	}
	return 0;
}

const Node* Tree::node( const char* name ) const {
	for ( int i = 0; i < mNodeNumber; ++i ){
		if ( *( mNodes[ i ].name() ) == name ){
			return &mNodes[ i ];
		}
	}
	return 0;
}

//アニメーションをセットする
void Tree::setAnimation( const Animation* anim ){
	for ( int i = 0; i < mNodeNumber; ++i ){
		const AnimationNode* an = anim->node( mNodes[ i ].name()->c_str() );
		mNodes[ i ].setAnimation( an ); //0でも気にせずセット。むしろしないとまずい。
	}
	mTime = 0.0; //時刻巻き戻し
}

//アニメーション状態を更新し、時刻を1進める。
void Tree::update(){
	for ( int i = 0; i < mNodeNumber; ++i ){
		mNodes[ i ].update( mTime );
	}
	mTime += 1.0;
}
