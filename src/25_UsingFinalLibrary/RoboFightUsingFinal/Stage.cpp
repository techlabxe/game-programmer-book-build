#include "GameLib/Framework.h"

#include "Stage.h"
using namespace GameLib::Scene;

Stage::Stage(){
	mSceneContainer = Container::create( "data/stage.txt" );
	while ( !mSceneContainer.isReady() ){
		;
	}
	mModel = mSceneContainer.createModel( "draw" );
}

Stage::~Stage(){
}

void Stage::draw() const {
	mModel.draw();
}
