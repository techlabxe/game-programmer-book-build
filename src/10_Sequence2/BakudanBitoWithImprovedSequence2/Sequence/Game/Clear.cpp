#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Sequence/Game/Clear.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/Game/Ready.h"
#include "Image.h"

namespace Sequence{
namespace Game{

Clear::Clear() : mImage( 0 ), mCount( 0 ){
	mImage = new Image( "data/image/dummy.dds" );
}

Clear::~Clear(){
	SAFE_DELETE( mImage );
}

Child* Clear::update( Parent* parent ){
	Child* next = this;
	if ( mCount == 60 ){ //1秒待つ 
		if ( parent->hasFinalStageCleared() ){ //最終面をクリアしたならエンディングへ
			parent->moveTo( Parent::NEXT_ENDING ); 
		}else{
			next = new Ready;
		}
	}
	//描画
	//まずゲーム画面を描画
	parent->drawState();
	//上に祝いのメッセージを描画
	mImage->draw();
	//クリアーと字を描く
	Framework::instance().drawDebugString( 0, 0, "ｸﾘｱｰ!" );

	++mCount;

	return next;
}

} //namespace Game
} //namespace Sequence
