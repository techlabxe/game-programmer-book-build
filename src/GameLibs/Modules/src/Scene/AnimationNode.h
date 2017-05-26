#ifndef INCLUDED_GAMELIB_SCENE_ANIMATIONNODE_H
#define INCLUDED_GAMELIB_SCENE_ANIMATIONNODE_H

#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
#include "GameLib/Scene/Container.h"
#include "GameLib/Math/Vector3.h"

#include "Scene/AnimationNode.h"
#include "Scene/Curve.h"

namespace GameLib{
using namespace GameLib::Math;
namespace Scene{

class AnimationNode{
public:
	//この関数はコンテナを使うので、インクルードループを避けるためにAnimationNode.cppにおいてある。
	AnimationNode( PseudoXml::ConstElement&, Container::Impl& );
	~AnimationNode(){
		mName = 0; //コンテナが持っているので参照を外すだけ
		for ( int i = 0; i < mCurveNumber; ++i ){
			mCurves[ i ].~Curve();
		}
		OPERATOR_DELETE( mCurves );
	}
	const Curve* curves( int i ) const {
		return &mCurves[ i ];
	}
	int curveNumber() const {
		return mCurveNumber;
	}
	void update(
	Vector3* translation,
	Vector3* rotation,
	Vector3* scale,
	float t ) const {
		for ( int i = 0; i < mCurveNumber; ++i ){
			const Curve& c = mCurves[ i ];
			float v = c( t );
			switch ( c.type() ){
				case Curve::TYPE_TX: translation->x = v; break;
				case Curve::TYPE_TY: translation->y = v; break;
				case Curve::TYPE_TZ: translation->z = v; break;
				case Curve::TYPE_RX: rotation->x = v; break;
				case Curve::TYPE_RY: rotation->y = v; break;
				case Curve::TYPE_RZ: rotation->z = v; break;
				case Curve::TYPE_SX: scale->x = v; break;
				case Curve::TYPE_SY: scale->y = v; break;
				case Curve::TYPE_SZ: scale->z = v; break;
				case Curve::TYPE_SA: *scale = v; break;
				default: ASSERT( false ); break;
			}
		}
	}
	const char* name() const {
		return mName;
	}
private:
	Curve* mCurves;
	int mCurveNumber;
	const char* mName;
};

} //namespace Scene
} //namespace GameLib

#endif

