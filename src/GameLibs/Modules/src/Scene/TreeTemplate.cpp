#include "GameLib/GameLib.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
#include "GameLib/Scene/Container.h"
#include "GameLib/Base/RefString.h"
#include "GameLib/Math/Functions.h"

#include "Scene/TreeTemplate.h"
#include "Scene/ContainerImpl.h"
#include "Scene/NodeTemplate.h"

using namespace GameLib;
using namespace GameLib::PseudoXml;
using namespace GameLib::Scene;

namespace GameLib{
namespace Scene{

TreeTemplate::TreeTemplate( ConstElement& e, Container::Impl& container ) :
mNodes( 0 ),
mNodeNumber( 0 ),
mName( 0 ),
mContainer( &container ){
	ASSERT( e.childNumber() == 1 && "Tree must have 1 root node. this data is invalid. " ); //ルートノードは一個しかないよね？つうか、一個はあるよね？
	//名前取得
	int an = e.attributeNumber();
	for ( int i = 0; i < an; ++i ){
		ConstAttribute a = e.attribute( i );
		if ( RefString( "name" ) == a.name() ){
			RefString value( a.value() );
			mName = container.allocateString( value );
		}
	}
	//まずノードを数えよう
	ConstElement root = e.child( 0 ); //最初の子がルート
	mNodeNumber = countNode( root, 0 );
	mNodes = NEW NodeTemplate[ mNodeNumber ];
	//階層を構築
	build( root, container, 0 );
}

//再帰
int TreeTemplate::countNode( ConstElement& e, int n ){
	++n; //自分の分プラス
	int cn = e.childNumber();
	for ( int i = 0; i < cn; ++i ){
		ConstElement c = e.child( i );
		n = countNode( c, n );
	}
	return n;
}

int TreeTemplate::build( ConstElement& e, Container::Impl& container, int nodePos ){
	//自分の情報を入れます
	bool hasTransform = false; //行列を持っていれば行列を優先
	Vector3 translation( 0.f, 0.f, 0.f );
	Vector3 rotation( 0.f, 0.f, 0.f );
	Vector3 scale( 1.f, 1.f, 1.f );

	int an = e.attributeNumber();
	NodeTemplate& node = mNodes[ nodePos ];
	for ( int i = 0; i < an; ++i ){
		ConstAttribute a = e.attribute( i );
		RefString name( a.name() );

		if ( name == "name" ){
			RefString value( a.value() );
			node.mName = container.allocateString( value );
		}else if ( name == "batch" ){
			node.mBatch = container.batch( a.value() );
		}else if ( name == "translation" ){
			a.getFloatValues( &translation.x, 3 );
		}else if ( name == "rotation" ){
			a.getFloatValues( &rotation.x, 3 );
		}else if ( name == "scale" ){
			a.getFloatValues( &scale.x, 3 );
		}else if ( name == "transform" ){
			a.getFloatValues( &node.mTransform.m00, 12 );
			hasTransform = true;
		}
	}
	if ( !hasTransform ){
		node.mTransform.setTranslation( translation );
		node.mTransform.rotateY( rotation.y );
		node.mTransform.rotateX( rotation.x );
		node.mTransform.rotateZ( rotation.z );
		node.mTransform.scale( scale );
	}
	++nodePos;
	ASSERT( nodePos <= mNodeNumber );  //ありえねえ
	//子供の情報を入れます
	int cn = e.childNumber();
	int elder = -1;
	for ( int i = 0; i < cn; ++i ){
		ConstElement c = e.child( i );
		if ( i == 0 ){ //0最初の子なら自分にセット
			node.mChild = nodePos;
		}else{ //それ以外なら前の子に
			mNodes[ elder ].mBrother = nodePos;
		}
		elder = nodePos;
		nodePos = build( c, container, nodePos );
	}
	return nodePos;
}

} //namespace Scene
} //namespace GameLib

