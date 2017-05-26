#include "GameLib/GameLib.h"
#include "GameLib/Base/RefString.h"
#include "GameLib/Scene/Animation.h"

#include "Scene/AnimationImpl.h"
#include "Scene/AnimationNode.h"
#include "Scene/ContainerImpl.h"

#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
using namespace GameLib;
using namespace GameLib::PseudoXml;

namespace GameLib{
namespace Scene{

Animation::Impl::Impl( PseudoXml::ConstElement& e, Container::Impl& c ) : 
mContainer( &c ){
	ASSERT( RefString( "Animation" ) == e.name() ); //アニメだよね？
	ASSERT( e.childNumber() > 0 ); //ノード一個はあるよね？
	//名前取得
	int an = e.attributeNumber();
	for ( int i = 0; i < an; ++i ){
		ConstAttribute a = e.attribute( i );
		if ( RefString( "name" ) == a.name() ){
			mName = c.allocateString( RefString( a.value() ) );
		}
	}
	mNodeNumber = e.childNumber();
	mNodes = static_cast< AnimationNode* >( OPERATOR_NEW( sizeof( AnimationNode ) * mNodeNumber ) );
	mNodeMap.setCapacity( mNodeNumber );
	for ( int i = 0; i < mNodeNumber; ++i ){
		ConstElement child = e.child( i );
		new ( &mNodes[ i ] ) AnimationNode( child, c );
		mNodeMap.add( mNodes[ i ].name(), i );
	}
}

Animation::Animation() : mImpl( 0 ){
}

Animation::Animation( const Animation& o ) : mImpl( o.mImpl ){
	if ( mImpl ){
		mImpl->container()->refer();
	}
}

Animation::~Animation(){
	release();
}

void Animation::release(){
	if ( mImpl ){
		Container::Impl* c = mImpl->container();
		c->release();
		if ( c->referenceCount() == 0 ){
			SAFE_DELETE( c );
		}
	}
	mImpl = 0;
}

Animation& Animation::operator=( const Animation& o ){
	release();
	mImpl = o.mImpl;
	if ( mImpl ){
		mImpl->container()->refer();
	}
	return *this;
}

bool Animation::operator==( const Animation& o ) const {
	return ( mImpl == o.mImpl );
}

bool Animation::operator!=( const Animation& o ) const {
	return ( mImpl != o.mImpl );
}

Animation::operator void*() const {
	return ( mImpl == 0 ) ? 0 : reinterpret_cast< void* >( ~0 );
}

Animation::Animation( Impl* impl ) : mImpl( impl ){
	if ( mImpl ){
		mImpl->container()->refer();
	}
}

} //namespace Scene
} //namespace GameLib

