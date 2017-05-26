#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Sequence/Game/Loading.h"
#include "Sequence/Game/Parent.h"
#include "Image.h"

namespace Sequence{
namespace Game{

//ロードを始めるのはupdateだからここではロードは開始しないが、
//「ロード中」という画像はここでロードする。
Loading::Loading() : 
mImage( 0 ),
mCount( 0 ),
mStarted( false ){
	mImage = new Image( "data/image/loading.dds" );
}

Loading::~Loading(){
	SAFE_DELETE( mImage );
}

void Loading::update( Parent* parent ){
	if ( !mStarted ){
		//結局ロードするのは親。なぜならStateを持っているのは親だから。
		parent->startLoading();
		mStarted = true;
	}else if ( mCount == 60 ){ //1秒待つ
		parent->moveTo( Parent::SEQ_PLAY ); //Playに移動
	}

	//描画
	mImage->draw();

	++mCount;
}

} //namespace Game
} //namespace Sequence