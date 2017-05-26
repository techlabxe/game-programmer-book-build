#include "GameLib/GameLib.h"
using namespace GameLib;

#include "Sequence/GameOver.h"
#include "Sequence/Parent.h"
#include "Image.h"

namespace Sequence{

GameOver::GameOver() : mImage( 0 ), mCount( 0 ){
	mImage = new Image( "data/image/gameover.tga" );
}

GameOver::~GameOver(){
	SAFE_DELETE( mImage );
}

void GameOver::update( Parent* parent ){
	if ( mCount == 60 ){ //1•b‘Ò‚Â 
		parent->moveTo( Parent::NEXT_TITLE );
	}
	//•`‰æ
	mImage->draw();

	++mCount;
}

} //namespace Sequence