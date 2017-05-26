#include "GameLib/GameLib.h"

#include "Sequence/Parent.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/StageSelect.h"
#include "Sequence/Title.h"
#include "StringRenderer.h"

namespace Sequence{

Parent::Parent() : 
mTitle( 0 ),
mStageSelect( 0 ),
mGame( 0 ),
mNext( SEQ_NONE ),
mStageID( 0 ){
	//文字列クラス初期化
	StringRenderer::create( "data/image/font.dds" );
	//最初に作るのはタイトル
	mTitle = new Title();
}

Parent::~Parent(){
	//残っていれば抹殺
	SAFE_DELETE( mTitle );
	SAFE_DELETE( mStageSelect );
	SAFE_DELETE( mGame );
	//文字列クラス終了
	StringRenderer::destroy();
}

void Parent::update(){
	if ( mTitle ){
		mTitle->update( this );
	}else if ( mStageSelect ){
		mStageSelect->update( this );
	}else if ( mGame ){
		mGame->update( this );
	}else{
		HALT( "bakana!" ); //ありえない
	}
	//遷移判定
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
			ASSERT( mStageID != 0 ); //ちゃんと入れてくれているはず
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