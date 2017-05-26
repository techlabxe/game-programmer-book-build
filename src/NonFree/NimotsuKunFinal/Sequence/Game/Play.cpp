#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"

#include "Sequence/Game/Play.h"
#include "Sequence/Game/Parent.h"
#include "State.h"

using namespace GameLib;

namespace Sequence{
namespace Game{

Play::Play(){ //何もしない
}

Play::~Play(){ //何もしない
}

//ゲーム本体
void Play::update( Parent* parent ){
	State* state = parent->getState();

	bool cleared = false;
	//メインループ
	//クリアチェック
	if ( state->hasCleared() ){
		cleared = true;
	}
	//入力取得
	//SPACE押されたらメニューへ行く
	Framework f = Framework::instance();
	if ( f.isKeyTriggered( ' ' ) ){
		parent->moveTo( Parent::SEQ_MENU );
	}else{ //通常更新
		int dx = 0;
		int dy = 0;
		if ( f.isKeyOn( 'a' ) ){
			dx -= 1;
		}else if ( f.isKeyOn( 's' ) ){
			dx += 1;
		}else if ( f.isKeyOn( 'w' ) ){
			dy -= 1;
		}else if ( f.isKeyOn( 'z' ) ){
			dy += 1;
		}
		//更新
		state->update( dx, dy );

		//クリアしたなら上へ報告
		if ( cleared ){
			parent->moveTo( Parent::SEQ_CLEAR );
		}
	}
	//描画
	state->draw();
}

} //namespace Game
} //namespace Sequence
