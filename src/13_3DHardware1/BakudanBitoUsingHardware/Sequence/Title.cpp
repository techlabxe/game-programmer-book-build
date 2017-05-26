#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Sequence/Title.h"
#include "Sequence/Parent.h"
#include "Image.h"
#include "Pad.h"
#include "SoundManager.h"

namespace Sequence{

Title::Title() : mImage( 0 ), mCursorPosition( 0 ){
	mImage = new Image( "data/image/dummy.dds" );
}

Title::~Title(){
	SAFE_DELETE( mImage );
}

void Title::update( Parent* parent ){
	//入力取得
	if ( Pad::isTriggered( Pad::U ) ){
		--mCursorPosition;
		if ( mCursorPosition < 0 ){ //マイナスは最大値にループ
 			mCursorPosition = 1;
		}
		//カーソル音
		SoundManager::instance()->playSe( SoundManager::SE_CURSOR_MOVE );
	}else if ( Pad::isTriggered( Pad::D ) ){
		++mCursorPosition;
		if ( mCursorPosition > 1 ){ //1を越えたら0にループ
			mCursorPosition = 0;
		}
		//カーソル音
		SoundManager::instance()->playSe( SoundManager::SE_CURSOR_MOVE );
	}else if ( Pad::isTriggered( Pad::A ) ){
		parent->moveTo( Parent::NEXT_GAME );
		if ( mCursorPosition == 0 ){
			parent->setMode( Parent::MODE_1P );
		}else if ( mCursorPosition == 1 ){
			parent->setMode( Parent::MODE_2P );
		}else{
			HALT( "arienai" );
		}
		//決定音
		SoundManager::instance()->playSe( SoundManager::SE_SELECTION );
	}
	//描画
	mImage->draw();
	//字を出す
	Framework f = Framework::instance();
	f.drawDebugString( 0, 0, "[ﾀｲﾄﾙ] : ﾊﾞｸﾀﾞﾝﾋﾞﾄ" );
	f.drawDebugString( 1, 2, "ﾋﾄﾘ ﾃﾞ ｱｿﾌﾞ" );
	f.drawDebugString( 1, 3, "ﾌﾀﾘ ﾃﾞ ｺﾛｼｱｳ" );
	//カーソルを書く
	f.drawDebugString( 0, mCursorPosition + 2, ">" );
}

} //namespace Sequence