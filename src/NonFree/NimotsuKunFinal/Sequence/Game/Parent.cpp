#include "GameLib/GameLib.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/Game/Loading.h"
#include "Sequence/Game/Clear.h"
#include "Sequence/Game/Menu.h"
#include "Sequence/Game/Play.h"
#include "Sequence/Parent.h"
#include "State.h"
#include "File.h"
#include <sstream>

namespace Sequence{
namespace Game{

Parent::Parent( int stageID ) : 
mState( 0 ),
mStageID( stageID ),
mNext( SEQ_NONE ),
mClear( 0 ),
mLoading( 0 ),
mMenu( 0 ),
mPlay( 0 ){
	//Å‰‚Éƒ[ƒh
	mLoading = new Loading();
}

Parent::~Parent(){
	SAFE_DELETE( mState );
	SAFE_DELETE( mClear );
	SAFE_DELETE( mLoading );
	SAFE_DELETE( mMenu );
	SAFE_DELETE( mPlay );
}

void Parent::update( GrandParent* parent ){
	if ( mClear ){
		mClear->update( this );
	}else if ( mLoading ){
		mLoading->update( this );
	}else if ( mMenu ){
		mMenu->update( this );
	}else if ( mPlay ){
		mPlay->update( this );
	}else{
		HALT( "bakana!" ); //‚ ‚è‚¦‚È‚¢
	}
	//‘JˆÚ”»’è
	switch ( mNext ){
		case SEQ_STAGE_SELECT:
			parent->moveTo( GrandParent::SEQ_STAGE_SELECT ); //ŠÛ“Š‚°
			break;
		case SEQ_TITLE:
			parent->moveTo( GrandParent::SEQ_TITLE ); //ŠÛ“Š‚°
			break;
		case SEQ_PLAY:
			SAFE_DELETE( mLoading );
			SAFE_DELETE( mMenu );
			mPlay = new Game::Play();
			break;
		case SEQ_CLEAR:
			SAFE_DELETE( mPlay );
			mClear = new Game::Clear();
			break;
		case SEQ_MENU:
			SAFE_DELETE( mPlay );
			mMenu = new Game::Menu();
			break;
	}
	mNext = SEQ_NONE; //‚±‚ê‚ð–Y‚ê‚é‚Æ‚Ð‚Ç‚¢‚±‚Æ‚É
}

void Parent::startLoading(){
	std::ostringstream oss;
	oss << "data/stageData/" << mStageID << ".txt";
	File file( oss.str().c_str() ); //‚±‚ê‚Åconst char*‚ªŽæ‚ê‚é
	mState = new State( file.getData(), file.getSize() );
}

void Parent::moveTo( SeqID next ){
	mNext = next;
}

State* Parent::getState(){
	return mState;
}

} //namespace Game
} //namespace Sequence