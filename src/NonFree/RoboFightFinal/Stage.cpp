#include "GameLib/Framework.h"
#include "GameLib/Math/Vector3.h"

#include "Stage.h"
using namespace std;
using namespace GameLib::Scene;

Stage::Stage(){
	mSceneContainer = Container::create( "data/skyNtile.x.txt" );
	while ( !mSceneContainer.isReady() ){
		;
	}
	mModel0 = mSceneContainer.createModel( "batch0_0" );
	mModel1 = mSceneContainer.createModel( "batch0_1" );
	Vector3 scale( 100.f );
	mModel0.setScale( scale );
	mModel1.setScale( scale );
}

Stage::~Stage(){
}

void Stage::draw() const {
	mModel0.draw();
	mModel1.draw();
}
