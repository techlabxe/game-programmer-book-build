#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Image.h"
#include "Pad.h"
#include "SoundManager.h"
#include "Sequence/Game/Pause.h"
#include "Sequence/Game/Parent.h"

namespace Sequence{
namespace Game{

Pause::Pause() : mImage( 0 ),mCursorPosition( 0 ){
	mImage = new Image( "data/image/dummy.dds" );
}

Pause::~Pause(){
	SAFE_DELETE( mImage );
}

void Pause::update( Parent* parent ){
	//0:やりなおし
	//1:面セレへ
	//2:タイトル画面へ
	//3:戻る
	if ( Pad::isTriggered( Pad::U) ){
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
		if ( mCursorPosition == 0 ){ //ゲームに戻る
			parent->moveTo( Parent::NEXT_PLAY );
		}else if ( mCursorPosition == 1 ){ //タイトルへ戻る
			parent->moveTo( Parent::NEXT_TITLE );
		}
		//決定音
		SoundManager::instance()->playSe( SoundManager::SE_SELECTION );
	}
	//描画
	//まずゲーム画面
	parent->drawState();
	//上に重ねる
	mImage->draw();
	//字を描く
	Framework f = Framework::instance();
	f.drawDebugString( 0, 0, "[ﾎﾟｰｽﾞ]" );
	f.drawDebugString( 1, 2, "ｹﾞｰﾑｦﾂﾂﾞｹﾙ" );
	f.drawDebugString( 1, 3, "ﾀｲﾄﾙﾆﾓﾄﾞﾙ" );
	//カーソルを書く
	f.drawDebugString( 0, mCursorPosition + 2, ">" );
}



} //namespace Game
} //namespace Sequence