#include "GameLib/GameLib.h"
#include "Library/Animation.h"
#include "Library/AnimationNode.h"

#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
using namespace GameLib;
using namespace GameLib::PseudoXml;

Animation::Animation( Element& e ) : 
mNodes( 0 ),
mNodeNumber( 0 ){
	ASSERT( string( "Animation" ) == e.name() ); //アニメだよね？
	ASSERT( e.childNumber() > 0 ); //ノード一個はあるよね？
	//名前取得
	int an = e.attributeNumber();
	const string nameStr( "name" );
	for ( int i = 0; i < an; ++i ){
		Attribute a = e.attribute( i );
		if ( nameStr == a.name() ){
			mName = a.value();
		}
	}
	mNodeNumber = e.childNumber();
	mNodes = new AnimationNode*[ mNodeNumber ];
	for ( int i = 0; i < mNodeNumber; ++i ){
		Element c = e.child( i );
		mNodes[ i ] = new AnimationNode( c );
	}
}

Animation::~Animation(){
	for ( int i = 0; i < mNodeNumber; ++i ){
		SAFE_DELETE( mNodes[ i ] );
	}
	SAFE_DELETE_ARRAY( mNodes );
}

const AnimationNode* Animation::node( const char* name ) const {
	for ( int i = 0; i < mNodeNumber; ++i ){
		if ( *( mNodes[ i ]->name() ) == name ){
			return mNodes[ i ];
		}
	}
	return 0;
}

const string* Animation::name() const {
	return &mName;
}

