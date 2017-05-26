#include "GameLib/Framework.h"

#include "Stage.h"
#include "Library/GraphicsDatabase.h"
#include "Library/Batch.h"
#include "Library/IndexBuffer.h"
#include "Library/VertexBuffer.h"
#include "Library/Model.h"
#include "Library/Matrix44.h"
#include "Library/Vector3.h"
#include <limits>
using namespace std;

Stage::Stage() : mDatabase( 0 ), mModel( 0 ){
	mDatabase = new GraphicsDatabase( "stage.txt" );
	mModel = mDatabase->createModel( "draw" );
}

Stage::~Stage(){
	SAFE_DELETE( mModel );
	SAFE_DELETE( mDatabase );
}

void Stage::draw( const Matrix44& pvm ) const {
	mModel->draw( pvm );
}
