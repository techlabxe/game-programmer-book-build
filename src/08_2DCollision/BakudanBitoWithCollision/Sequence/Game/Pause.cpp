#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Image.h"
#include "Sequence/Game/Pause.h"
#include "Sequence/Game/Parent.h"

namespace Sequence{
namespace Game{

Pause::Pause() : mImage( 0 ),mCursorPosistion( 0 ){
	mImage = new Image( "data/image/dummy.dds" );
}

Pause::~Pause(){
	SAFE_DELETE( mImage );
}

void Pause::update( Parent* parent ){
	Framework f = Framework::instance();;
	//0:やりなおし
	//1:面セレへ
	//2:タイトル画面へ
	//3:戻る
	if ( f.isKeyTriggered( 'w' ) ){
		--mCursorPosistion;
		if ( mCursorPosistion < 0 ){ //マイナスは最大値にループ
 			mCursorPosistion = 1;
		}
	}else if ( f.isKeyTriggered( 'z' ) ){
		++mCursorPosistion;
		if ( mCursorPosistion > 1 ){ //1を越えたら0にループ
			mCursorPosistion = 0;
		}
	}else if ( f.isKeyTriggered( ' ' ) ){
		if ( mCursorPosistion == 0 ){ //ゲームに戻る
			parent->moveTo( Parent::NEXT_PLAY );
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
	f.drawDebugString( 0, 0, "[ﾎﾟｰｽﾞ]" );
	f.drawDebugString( 1, 2, "ｹﾞｰﾑｦﾂﾂﾞｹﾙ" );
	f.drawDebugString( 1, 3, "ﾀｲﾄﾙﾆﾓﾄﾞﾙ" );
	//カーソルを書く
	f.drawDebugString( 0, mCursorPosistion + 2, ">" );
}



} //namespace Game
} //namespace Sequence