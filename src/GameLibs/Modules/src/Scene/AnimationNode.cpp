#include "GameLib/GameLib.h"
#include "GameLib/Base/RefString.h"
#include "Scene/AnimationNode.h"
#include "Scene/ContainerImpl.h"
#include "Scene/Curve.h"

#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"

namespace GameLib{
namespace Scene{
	
AnimationNode::AnimationNode( PseudoXml::ConstElement& e, Container::Impl& c ){
	ASSERT( RefString( "Node" ) == e.name() ); //Nodeだよね？
	ASSERT( e.childNumber() > 0 ); //カーブ一個はあるよね？
	//名前取得
	int an = e.attributeNumber();
	for ( int i = 0; i < an; ++i ){
		ConstAttribute a = e.attribute( i );
		if ( RefString( "name" ) == a.name() ){
			mName = c.allocateString( RefString( a.value() ) );
		}
	}
	mCurveNumber = e.childNumber();
	mCurves = static_cast< Curve* >( OPERATOR_NEW( sizeof( Curve ) * mCurveNumber ) );
	for ( int i = 0; i < mCurveNumber; ++i ){
		ConstElement child = e.child( i );
		new ( &mCurves[ i ] ) Curve( child );
	}
}

} //namespace Scene
} //namespace GameLib
