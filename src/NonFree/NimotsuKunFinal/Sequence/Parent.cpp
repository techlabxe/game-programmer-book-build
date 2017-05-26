#include "GameLib/GameLib.h"

#include "Sequence/Parent.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/StageSelect.h"
#include "Sequence/Title.h"

namespace Sequence{

Parent::Parent() : 
mTitle( 0 ),
mStageSelect( 0 ),
mGame( 0 ),
mNext( SEQ_NONE ),
mStageID( 0 ){
	//Å‰‚Éì‚é‚Ì‚Íƒ^ƒCƒgƒ‹
	mTitle = new Title();
}

Parent::~Parent(){
	//Žc‚Á‚Ä‚¢‚ê‚Î–•ŽE
	SAFE_DELETE( mTitle );
	SAFE_DELETE( mStageSelect );
	SAFE_DELETE( mGame );
}

void Parent::update(){
	if ( mTitle ){
		mTitle->update( this );
	}else if ( mStageSelect ){
		mStageSelect->update( this );
	}else if ( mGame ){
		mGame->update( this );
	}else{
		HALT( "bakana!" ); //‚ ‚è‚¦‚È‚¢
	}
	//‘JˆÚ”»’è
	switch ( mNext ){
		case SEQ_STAGE_SELECT:
			SAFE_DELETE( mTitle );
			SAFE_DELETE( mGame );
			mStageSelect = new StageSelect();
			break;
		case SEQ_TITLE:
			SAFE_DELETE( mGame );
			mTitle = new Title(); 
			break;
		case SEQ_GAME:
			SAFE_DELETE( mStageSelect );
			ASSERT( mStageID != 0 ); //‚¿‚á‚ñ‚Æ“ü‚ê‚Ä‚­‚ê‚Ä‚¢‚é‚Í‚¸
			mGame = new Game::Parent( mStageID );
			break;
	}
	mNext = SEQ_NONE;
}


void Parent::moveTo( SeqID next ){
	mNext = next;
}

void Parent::setStageID( int stageID ){
	mStageID = stageID;
}

} //namespace Sequence