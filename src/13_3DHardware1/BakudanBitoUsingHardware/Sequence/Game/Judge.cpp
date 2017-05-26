#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Image.h"
#include "Pad.h"
#include "Sequence/Game/Judge.h"
#include "Sequence/Game/Parent.h"

namespace Sequence{
namespace Game{

Judge::Judge() : mImage( 0 ), mCursorPosition( 0 ){
	mImage = new Image( "data/image/dummy.dds" );
}

Judge::~Judge(){
	SAFE_DELETE( mImage );
}

void Judge::update( Parent* parent ){
	if ( Pad::isTriggered( Pad::U ) ){
		--mCursorPosition;
		if ( mCursorPosition < 0 ){ //マイナスは最大値にループ
 			mCursorPosition = 1;
		}
	}else if ( Pad::isTriggered( Pad::D ) ){
		++mCursorPosition;
		if ( mCursorPosition > 1 ){ //1を越えたら0にループ
			mCursorPosition = 0;
		}
	}else if ( Pad::isTriggered( Pad::A ) ){
		if ( mCursorPosition == 0 ){
			//続けて勝負
			parent->moveTo( Parent::NEXT_READY );
		}else if ( mCursorPosition == 1 ){
			//タイトルへ
			parent->moveTo( Parent::NEXT_TITLE );
		}
	}
	//描画
	//まずゲーム画面
	parent->drawState();
	//上に重ねる
	mImage->draw();

	//まずどっちが勝ったのか表示
	Framework f = Framework::instance();
	f.drawDebugString( 0, 0, "[ｼｮｳﾊｲｹｯﾃｲ]" );

	Parent::PlayerID winner = parent->winner();
	if ( winner == Parent::PLAYER_1 ){
		f.drawDebugString( 0, 1, "1Pﾉ ｶﾁ!" );
	}else if ( winner == Parent::PLAYER_2 ){
		f.drawDebugString( 0, 1, "2Pﾉ ｶﾁ!" );
	}else{
		f.drawDebugString( 0, 1, "ｼｮｳｼｬﾅｼ! ﾋｷﾜｹ!" );
	}
	//めぬー
	f.drawDebugString( 1, 3, "ﾏﾀﾞ ｺﾛｼｱｳ" );
	f.drawDebugString( 1, 4, "ﾔﾒﾃ ﾀｲﾄﾙ ｶﾞﾒﾝ ﾍ" );
	//カーソルを書く
	f.drawDebugString( 0, mCursorPosition + 3, ">" );
}



} //namespace Game
} //namespace Sequence