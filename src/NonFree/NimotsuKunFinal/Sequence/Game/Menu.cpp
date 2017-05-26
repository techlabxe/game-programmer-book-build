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
	mImage = new Image( "data/image/menu.dds" );
}

Menu::~Menu(){
	SAFE_DELETE( mImage );
}

void Menu::update( Parent* parent ){
	//1:やりなおし
	//2:面セレへ
	//3:タイトル画面へ
	//4:戻る
	//添え字がずれると面倒なので0も入れておく
	char numberChars[] = { '0', '1', '2', '3', '4' };
	int inputNumber = 0;
	for ( int i = 0; i < 5; ++i ){
		if ( Framework::instance().isKeyTriggered( numberChars[ i ] ) ){
			inputNumber = i;
		}
	}
	switch ( inputNumber ){
		case 1: //やりなおし
			parent->getState()->reset();
			parent->moveTo( Parent::SEQ_PLAY );
			break;
		case 2: //面セレへ
			parent->moveTo( Parent::SEQ_STAGE_SELECT );
			break;
		case 3: //タイトルへ
			parent->moveTo( Parent::SEQ_TITLE );
			break;
		case 4: //そのまま戻る
			parent->moveTo( Parent::SEQ_PLAY );
			break;
		default: //その他は無視
			break;
	}

	//描画
	//まずゲーム画面
	parent->getState()->draw();
	//上に重ねる
	mImage->draw();
}



} //namespace Game
} //namespace Sequence