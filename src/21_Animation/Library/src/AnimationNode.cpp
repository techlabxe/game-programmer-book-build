#include "GameLib/GameLib.h"
#include "Library/AnimationNode.h"
#include "Library/Curve.h"
#include "Library/Vector3.h"

#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
using namespace GameLib;
using namespace GameLib::PseudoXml;

AnimationNode::AnimationNode( Element& e ) :
mCurves( 0 ),
mCurveNumber( 0 ){
	ASSERT( string( "Node" ) == e.name() ); //Nodeだよね？
	ASSERT( e.childNumber() > 0 ); //カーブ一個はあるよね？
	//名前取得
	int an = e.attributeNumber();
	const string nameStr( "name" );
	for ( int i = 0; i < an; ++i ){
		Attribute a = e.attribute( i );
		if ( nameStr == a.name() ){
			mName = a.value();
		}
	}
	mCurveNumber = e.childNumber();
	mCurves = new Curve*[ mCurveNumber ];
	for ( int i = 0; i < mCurveNumber; ++i ){
		Element c = e.child( i );
		mCurves[ i ] = new Curve( c );
	}
}

AnimationNode::~AnimationNode(){
	for ( int i = 0; i < mCurveNumber; ++i ){
		SAFE_DELETE( mCurves[ i ] );
	}
	SAFE_DELETE_ARRAY( mCurves );
}

void AnimationNode::data(
Vector3* translation,
Vector3* rotation,
Vector3* scale,
double time ) const {
	for ( int i = 0; i < mCurveNumber; ++i ){
		const Curve* c = mCurves[ i ];
		double v = c->get( time );
		Curve::Type t = c->type();
		switch ( t ){
			case Curve::TYPE_TRANSLATION_X: translation->x = v; break;
			case Curve::TYPE_TRANSLATION_Y: translation->y = v; break;
			case Curve::TYPE_TRANSLATION_Z: translation->z = v; break;
			case Curve::TYPE_ROTATION_X: rotation->x = v; break;
			case Curve::TYPE_ROTATION_Y: rotation->y = v; break;
			case Curve::TYPE_ROTATION_Z: rotation->z = v; break;
			case Curve::TYPE_SCALE_X: scale->x = v; break;
			case Curve::TYPE_SCALE_Y: scale->y = v; break;
			case Curve::TYPE_SCALE_Z: scale->z = v; break;
			default: ASSERT( false ); break; //ありない
		}
	}
}

const string* AnimationNode::name() const {
	return &mName;
}
