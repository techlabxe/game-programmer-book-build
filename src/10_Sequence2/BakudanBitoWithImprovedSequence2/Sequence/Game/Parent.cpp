#include "GameLib/GameLib.h"
#include "Sequence/Parent.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/Game/Ready.h"
#include "Sequence/Title.h"
#include "Sequence/GameOver.h"
#include "Sequence/Ending.h"
#include "Game/State.h"
#include "File.h"
#include <sstream>

namespace Sequence{
namespace Game{

Parent::Parent( GrandParent::Mode mode ) : 
mState( 0 ),
mStageID( 0 ),
mLife( INITIALI_LIFE_NUMBER ),
mNextSequence( NEXT_NONE ),
mChild( 0 ){
	if ( mode == GrandParent::MODE_1P ){
		mStageID = 1;
	}else{
		mStageID = 0;
	}
	//最初はReady
	mChild = new Ready();
}

Parent::~Parent(){
	SAFE_DELETE( mState );
	SAFE_DELETE( mChild );
}

Sequence::Child* Parent::update( GrandParent* ){
	Sequence::Child* next = this;
	Sequence::Game::Child* nextChild = mChild->update( this );
	//遷移判定
	if ( nextChild != mChild ){
		SAFE_DELETE( mChild );
		mChild = nextChild;
	}
	nextChild = 0; //念のため

	switch ( mNextSequence ){
		case NEXT_ENDING:
			next = new Ending;
			break;
		case NEXT_GAME_OVER:
			next = new GameOver;
			break;
		case NEXT_TITLE:
			next = new Title;
			break;
	}
	mNextSequence = NEXT_NONE;

	return next;
}

void Parent::moveTo( NextSequence next ){
	ASSERT( mNextSequence == NEXT_NONE );
	mNextSequence = next;
}

State* Parent::state(){
	return mState;
}

bool Parent::hasFinalStageCleared() const {
	return ( mStageID > FINAL_STAGE );
}

int Parent::lifeNumber() const {
	return mLife;
}

//Parent::ModeをParent::Modeに変換。下流シーケンスにParentを見せない。
Parent::Mode Parent::mode() const {
	Mode r = MODE_NONE;
	switch ( GrandParent::instance()->mode() ){
		case GrandParent::MODE_1P: r = MODE_1P; break;
		case GrandParent::MODE_2P: r = MODE_2P; break;
		default: ASSERT( false ); break;
	}
	return r;
}

void Parent::startLoading(){
	SAFE_DELETE( mState );
	mState = new State( mStageID );
}

void Parent::drawState() const {
	mState->draw();
}

Parent::PlayerID Parent::winner() const {
	return mWinner;
}

void Parent::setWinner( PlayerID id ){
	mWinner = id;
}

void Parent::goToNextStage(){
	++mStageID;
}

void Parent::reduceLife(){
	--mLife;
}

} //namespace Game
} //namespace Sequence