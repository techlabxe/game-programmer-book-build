#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Image.h"
#include "Pad.h"
#include "Sequence/Game/Pause.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/Game/Play.h"

namespace Sequence{
namespace Game{

Pause::Pause() : mImage( 0 ),mCursorPosistion( 0 ){
	mImage = new Image( "data/image/dummy.dds" );
}

Pause::~Pause(){
	SAFE_DELETE( mImage );
}

Child* Pause::update( Parent* parent ){
	Child* next = this;
	//0:やりなおし
	//1:面セレへ
	//2:タイトル画面へ
	//3:戻る
	if ( Pad::isTriggered( Pad::U) ){
		--mCursorPosistion;
		if ( mCursorPosistion < 0 ){ //マイナスは最大値にループ
 			mCursorPosistion = 1;
		}
	}else if ( Pad::isTriggered( Pad::D ) ){
		++mCursorPosistion;
		if ( mCursorPosistion > 1 ){ //1を越えたら0にループ
			mCursorPosistion = 0;
		}
	}else if ( Pad::isTriggered( Pad::A ) ){
		if ( mCursorPosistion == 0 ){ //ゲームに戻る
			next = new Play;
		}else if ( mCursorPosistion == 1 ){ //タイトルへ戻る
			parent->moveTo( Parent::NEXT_TITLE );
		}
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
	f.drawDebugString( 0, mCursorPosistion + 2, ">" );

	return next;
}



} //namespace Game
} //namespace Sequence