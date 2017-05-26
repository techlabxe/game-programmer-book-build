#include "GameLib/Framework.h"

#include "Stage.h"
#include "Library/GraphicsDatabase.h"
#include "Library/Batch.h"
#include "Library/IndexBuffer.h"
#include "Library/VertexBuffer.h"
#include "Library/Model.h"
#include <limits>
using namespace std;

Stage::Stage() : mDatabase( 0 ), mModel( 0 ){
	mDatabase = NEW GraphicsDatabase( "stage.txt" );
	mModel = mDatabase->createModel( "draw" );
}

Stage::~Stage(){
}

void Stage::draw() const {
	mModel->draw();
}
