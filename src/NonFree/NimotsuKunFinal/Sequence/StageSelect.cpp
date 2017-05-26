#include "GameLib/GameLib.h"
using namespace GameLib;

#include "GameLib/Framework.h"
#include "Sequence/StageSelect.h"
#include "Sequence/Parent.h"
#include "Image.h"
#include "sstream"
using namespace std;

namespace Sequence{

StageSelect::StageSelect() : mImage( 0 ), mCursorPosition( 0 ){
	mImage = new Image( "data/image/stageSelect.dds" );
}

StageSelect::~StageSelect(){
	SAFE_DELETE( mImage );
}

void StageSelect::update( Parent* parent ){
	//“ü—ÍŽæ“¾
	int stage = 0;
	//“Y‚¦Žš‚ª‚¸‚ê‚é‚Æ–ï‰î‚È‚Ì‚Å0‚à“ü‚ê‚Ä‚¨‚­
	char numberChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	for ( int i = 0; i < 10; ++i ){
		if ( Framework::instance().isKeyTriggered( numberChars[ i ] ) ){
			stage = i;
		}
	}
	if ( stage != 0 ){ //‚±‚ê‚ÅŽŸ‚Ös‚­
		parent->setStageID( stage );
		parent->moveTo( Parent::SEQ_GAME );
	}
	//•`‰æ
	mImage->draw();
}

} //namespace Sequence