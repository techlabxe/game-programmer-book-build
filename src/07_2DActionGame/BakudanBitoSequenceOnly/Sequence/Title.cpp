#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Sequence/Title.h"
#include "Sequence/Parent.h"
#include "Image.h"

namespace Sequence{

Title::Title() : mImage( 0 ), mCursorPosistion( 0 ){
	mImage = new Image( "data/image/dummy.dds" );
}

Title::~Title(){
	SAFE_DELETE( mImage );
}

void Title::update( Parent* parent ){
	//入力取得
	Framework f = Framework::instance();;
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
		parent->moveTo( Parent::NEXT_GAME );
		if ( mCursorPosistion == 0 ){
			parent->setMode( Parent::MODE_1P );
		}else if ( mCursorPosistion == 1 ){
			parent->setMode( Parent::MODE_2P );
		}else{
			HALT( "arienai" );
		}
	}
	//描画
	mImage->draw();
	//字を出す
	f.drawDebugString( 0, 0, "[ﾀｲﾄﾙ] : ﾊﾞｸﾀﾞﾝﾋﾞﾄ" );
	f.drawDebugString( 1, 2, "ﾋﾄﾘ ﾃﾞ ｱｿﾌﾞ" );
	f.drawDebugString( 1, 3, "ﾌﾀﾘ ﾃﾞ ｺﾛｼｱｳ" );
	//カーソルを書く
	f.drawDebugString( 0, mCursorPosistion + 2, ">" );
}

} //namespace Sequence