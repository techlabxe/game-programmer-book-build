#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Sequence/GameOver.h"
#include "Sequence/Parent.h"
#include "Sequence/Title.h"
#include "Image.h"

namespace Sequence{

GameOver::GameOver() : mImage( 0 ), mCount( 0 ){
	mImage = new Image( "data/image/dummy.dds" );
}

GameOver::~GameOver(){
	SAFE_DELETE( mImage );
}

Base* GameOver::update( Parent* ){
	Base* next = this;
	if ( mCount == 60 ){ //1•b‘Ò‚Â 
		next = new Title;
	}
	//•`‰æ
	mImage->draw();
	//š‚ğo‚·
	Framework::instance().drawDebugString( 0, 0, "¹Ş°Ñµ°ÊŞ° »ŞİÈİÃŞ¼ÀÈ" );

	++mCount;

	return next;
}

} //namespace Sequence