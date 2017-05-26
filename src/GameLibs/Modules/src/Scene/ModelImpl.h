#ifndef INCLUDED_GAMELIB_SCENE_MODELIMPL_H
#define INCLUDED_GAMELIB_SCENE_MODELIMPL_H

#include "GameLib/Base/Impl/ReferenceType.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Base/Impl/ReferenceType.h"
#include "GameLib/Graphics/Manager.h"

#include "Scene/BatchImpl.h"
#include "Scene/ContainerImpl.h"

namespace GameLib{
using namespace GameLib::Graphics;
namespace Scene{

class Model::Impl : public ReferenceType{
public:
	Impl( Batch::Impl* b ) : 
	mBatch( b ),
	mPosition( 0.f, 0.f, 0.f ),
	mAngle( 0.f, 0.f, 0.f ),
	mScale( 1.f, 1.f, 1.f ),
	mColor( 1.f, 1.f, 1.f ),
	mTransparency( 1.f ){
		mBatch->container()->refer();
	}
	~Impl(){
		Container::Impl* c = mBatch->container();
		c->release();
		if ( c->referenceCount() == 0 ){
			SAFE_DELETE( c );
		}
		mBatch = 0;
	}
	void draw(){
		ASSERT( mBatch && "Model::draw() : no Batch specified!" );
		
		//ƒ[ƒ‹ƒhs—ñ‚ðì‚é
		Matrix34 w;
		w.setTranslation( mPosition );
		w.rotateY( mAngle.y );
		w.rotateX( mAngle.x );
		w.rotateZ( mAngle.z );
		w.scale( mScale );
		Graphics::Manager().setWorldMatrix( w );
		//•`‰æ
		mBatch->draw( mColor, mTransparency );
	}
	Batch::Impl* mBatch;
	Vector3 mPosition;
	Vector3 mAngle;
	Vector3 mScale;
	Vector3 mColor;
	float mTransparency;
};

} //namespace Scene
} //namespace GameLib

#endif