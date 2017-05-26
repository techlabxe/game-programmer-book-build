#include "GameLib/GameLib.h"
#include "Sequence/Parent.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/Game/Clear.h"
#include "Sequence/Game/Ready.h"
#include "Sequence/Game/Pause.h"
#include "Sequence/Game/Play.h"
#include "Sequence/Game/Failure.h"
#include "Sequence/Game/Judge.h"
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
mClear( 0 ),
mReady( 0 ),
mPause( 0 ),
mPlay( 0 ),
mFailure( 0 ),
mJudge( 0 ){
	if ( mode == GrandParent::MODE_1P ){
		mStageID = 1;
	}else{
		mStageID = 0;
	}
	//最初はReady
	mReady = new Ready();
}

Parent::~Parent(){
	SAFE_DELETE( mState );
	SAFE_DELETE( mClear );
	SAFE_DELETE( mReady );
	SAFE_DELETE( mPause );
	SAFE_DELETE( mPlay );
	SAFE_DELETE( mFailure );
	SAFE_DELETE( mJudge );
}

void Parent::update( GrandParent* parent ){
	if ( mClear ){
		mClear->update( this );
	}else if ( mReady ){
		mReady->update( this );
	}else if ( mPause ){
		mPause->update( this );
	}else if ( mPlay ){
		mPlay->update( this );
	}else if ( mFailure ){
		mFailure->update( this );
	}else if ( mJudge ){
		mJudge->update( this );
	}else{
		HALT( "bakana!" ); //ありえない
	}
	//遷移判定
	switch ( mNextSequence ){
		case NEXT_CLEAR:
			STRONG_ASSERT( !mClear && !mReady && !mPause && mPlay && !mFailure && !mJudge );
			SAFE_DELETE( mPlay );
			mClear = new Clear();
			++mStageID; //次のステージへ
			break;
		case NEXT_READY:
			STRONG_ASSERT( !mReady && !mPause && !mPlay && ( mFailure || mClear ||  mJudge ) );
			SAFE_DELETE( mFailure );
			SAFE_DELETE( mClear );
			SAFE_DELETE( mJudge );
			mReady = new Ready();
			break;
		case NEXT_PAUSE:
			STRONG_ASSERT( !mClear && !mReady && !mPause && mPlay && !mFailure && !mJudge );
			SAFE_DELETE( mPlay );
			mPause = new Pause();
			break;
		case NEXT_PLAY:
			STRONG_ASSERT( !mClear && ( mReady || mPause ) && !mPlay && !mFailure && !mJudge );
			SAFE_DELETE( mReady );
			SAFE_DELETE( mPause );
			mPlay = new Play();
			break;
		case NEXT_FAILURE:
			STRONG_ASSERT( !mClear && !mReady && !mPause && mPlay && !mFailure && !mJudge );
			SAFE_DELETE( mPlay );
			mFailure = new Failure();
			--mLife;
			break;
		case NEXT_JUDGE:
			STRONG_ASSERT( !mClear && !mReady && !mPause && mPlay && !mFailure && !mJudge );
			SAFE_DELETE( mPlay );
			mJudge = new Judge();
			break;
		case NEXT_ENDING:
			STRONG_ASSERT( mClear && !mReady && !mPause && !mPlay && !mFailure && !mJudge );
			SAFE_DELETE( mClear );
			parent->moveTo( GrandParent::NEXT_ENDING );
			break;
		case NEXT_GAME_OVER:
			STRONG_ASSERT( !mClear && !mReady && !mPause && !mPlay && mFailure && !mJudge );
			SAFE_DELETE( mFailure );
			parent->moveTo( GrandParent::NEXT_GAME_OVER );
			break;
		case NEXT_TITLE:
			STRONG_ASSERT( !mClear && !mReady && ( mPause || mJudge ) && !mPlay && !mFailure );
			SAFE_DELETE( mPause );
			SAFE_DELETE( mJudge );
			parent->moveTo( GrandParent::NEXT_TITLE );
			break;
	}
	mNextSequence = NEXT_NONE;
}

void Parent::moveTo( NextSequence next ){
	STRONG_ASSERT( mNextSequence == NEXT_NONE );
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
		default: STRONG_ASSERT( false ); break;
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

} //namespace Game
} //namespace Sequence