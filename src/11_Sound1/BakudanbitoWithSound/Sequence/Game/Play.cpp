#include "GameLib/GameLib.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
using namespace GameLib;

#include "Sequence/Game/Play.h"
#include "Sequence/Game/Parent.h"
#include "Game/State.h"
#include "Pad.h"
#include "SoundManager.h"

namespace Sequence{
namespace Game{

Play::Play(){
	//BGM開始
	SoundManager::instance()->playBgm( SoundManager::BGM_GAME );
}

Play::~Play(){ 
	//BGM止める
	SoundManager::instance()->stopBgm();
}

//ゲーム本体
void Play::update( Parent* parent ){
	Input::Keyboard kb = Input::Manager::instance().keyboard();

	State* state = parent->state();

	bool cleared = state->hasCleared();
	bool die1P = !state->isAlive( 0 );
	bool die2P = !state->isAlive( 1 );
	//とりあえずデバグコマンドでテストする。
	if ( kb.isTriggered( '1' ) ){ //2P殺し
		die2P = true;
	}else if ( kb.isTriggered( '2' ) ){ //1P殺し
		die1P = true;
	}else if ( kb.isTriggered( 'c' ) ){
		cleared = true;
	}
	//SPACE押されたらポーズへ行く
	//クリアしたなら上へ報告
	if ( parent->mode() == Parent::MODE_1P ){
		if ( cleared && !die1P ){
			parent->moveTo( Parent::NEXT_CLEAR );
		}else if ( die1P ){
			parent->moveTo( Parent::NEXT_FAILURE );
		}
	}else{ //二人用
		if ( die1P || die2P ){ //どっちか死んでれば勝敗判定
			parent->moveTo( Parent::NEXT_JUDGE );
			if ( die1P && die2P ){
				parent->setWinner( Parent::PLAYER_NONE ); //両方死亡
			}else if ( die1P ){
				parent->setWinner( Parent::PLAYER_2 );
			}else{
				parent->setWinner( Parent::PLAYER_1 );
			}
		}
	}
	//Bボタンでポーズへ行く
	if ( Pad::isTriggered( Pad::B ) ){
		parent->moveTo( Parent::NEXT_PAUSE );
	}
	//更新
	state->update();
	//描画
	state->draw();
}

} //namespace Game
} //namespace Sequence
