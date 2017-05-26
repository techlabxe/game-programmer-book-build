#include "GameLib/GameLib.h"
using namespace GameLib;

#include "GameLib/Framework.h"
#include "Image.h"
#include "State.h"
#include "Sequence/Game/Menu.h"
#include "Sequence/Game/Parent.h"

namespace Sequence{
namespace Game{

Menu::Menu() : mImage( 0 ), mMenuCursor( 0 ){
	mImage = new Image( "data/image/dummy.dds" );
}

Menu::~Menu(){
	SAFE_DELETE( mImage );
}

void Menu::update( Parent* parent ){
	Framework f = Framework::instance();
	//0:やりなおし
	//1:面セレへ
	//2:タイトル画面へ
	//3:戻る
	if ( f.isKeyTriggered( 'w' ) ){
		--mMenuCursor;
		if ( mMenuCursor < 0 ){ //マイナスは最大値にループ
 			mMenuCursor = 3;
		}
	}else if ( f.isKeyTriggered( 'z' ) ){
		++mMenuCursor;
		if ( mMenuCursor > 3 ){ //3を越えたら0にループ
			mMenuCursor = 0;
		}
	}else if ( f.isKeyTriggered( ' ' ) ){
		switch ( mMenuCursor ){
			case 0: //やりなおし
				parent->state()->reset();
				parent->moveTo( Parent::SEQ_PLAY );
				break;
			case 1: //面セレへ
				parent->moveTo( Parent::SEQ_STAGE_SELECT );
				break;
			case 2: //タイトルへ
				parent->moveTo( Parent::SEQ_TITLE );
				break;
			case 3: //そのまま戻る
				parent->moveTo( Parent::SEQ_PLAY );
				break;
			default: //その他は無視
				break;
		}
	}
	//描画
	//まずゲーム画面
	parent->state()->draw();
	//上に重ねる
	mImage->draw();
	//字を描く
	f.drawDebugString( 0, 0, "[ﾒﾆｭｰ]" );
	f.drawDebugString( 1, 1, "ﾓｳｲﾁﾄﾞ" );
	f.drawDebugString( 1, 2, "ｽﾃｰｼﾞｾﾚｸﾄﾍ" );
	f.drawDebugString( 1, 3, "ﾀｲﾄﾙｶﾞﾒﾝﾍ" );
	f.drawDebugString( 1, 4, "ｹﾞｰﾑﾆﾓﾄﾞﾙ" );
	//カーソルを書く
	f.drawDebugString( 0, mMenuCursor + 1, ">" );
}



} //namespace Game
} //namespace Sequence