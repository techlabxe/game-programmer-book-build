#include "GameLib/GameLib.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
using namespace GameLib;

#include "Sequence/Game/Play.h"
#include "Sequence/Game/Clear.h"
#include "Sequence/Game/Failure.h"
#include "Sequence/Game/Judge.h"
#include "Sequence/Game/Pause.h"
#include "Sequence/Game/Parent.h"
#include "Game/State.h"
#include "Pad.h"

namespace Sequence{
namespace Game{

Play::Play(){ //何もしない
}

Play::~Play(){ //何もしない
}

//ゲーム本体
Child* Play::update( Parent* parent ){
	Child* next = this;
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
			parent->goToNextStage();
			next = new Clear;
		}else if ( die1P ){
			parent->reduceLife();
			next = new Failure;
		}
	}else{ //二人用
		if ( die1P || die2P ){ //どっちか死んでれば勝敗判定
			next = new Judge;
			if ( die1P && die2P ){
				parent->setWinner( Parent::PLAYER_NONE ); //両方死亡
			}else if ( die1P ){
				parent->setWinner( Parent::PLAYER_2 );
			}else{
				parent->setWinner( Parent::PLAYER_1 );
			}
		}
	}
	if ( ( next == this ) && Pad::isTriggered( Pad::B ) ){ //Bボタンでポーズへ行く。ただしまだ移ってない時だけだ
		next = new Pause;
	}
	//更新
	state->update();
	//描画
	state->draw();

	return next;
}

} //namespace Game
} //namespace Sequence
