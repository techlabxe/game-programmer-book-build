#include "GameLib/Framework.h"

#include "Stage.h"
#include "Library/GraphicsDatabase.h"
#include "Library/Model.h"
#include "Library/Matrix44.h"
#include "Library/Vector3.h"
#include "Library/Texture.h"

Stage::Stage() : mDatabase( 0 ), mModel( 0 ){
	mDatabase = new GraphicsDatabase( "stage.txt" );
	mModel = mDatabase->createModel( "stage" );
	mSphere.setPosition( Vector3( 0.0, -10000.0, 0.0 ) ); //1000メートル地下に中心
	mSphere.setRadius( 10000.0 );
}

Stage::~Stage(){
	SAFE_DELETE( mModel );
	SAFE_DELETE( mDatabase );
}

void Stage::draw( const Matrix44& pvm ) const {
	mModel->draw( pvm );
}

const Sphere* Stage::sphere() const {
	return &mSphere;
}
