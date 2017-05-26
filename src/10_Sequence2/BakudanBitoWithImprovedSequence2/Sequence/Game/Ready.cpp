#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Sequence/Game/Ready.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/Game/Play.h"
#include "Image.h"

namespace Sequence{
namespace Game{

Ready::Ready() : 
mImage( 0 ), mCount( 0 ), mStarted( false ){
	mImage = new Image( "data/image/dummy.dds" );
}

Ready::~Ready(){
	SAFE_DELETE( mImage );
}

Child* Ready::update( Parent* parent ){
	Child* next = this;
	if ( !mStarted ){
		parent->startLoading();
		mStarted = true;
	}
	if ( mCount >= 120 ){ //2秒経ったらPlayへ
		next = new Play;
	}else if ( mCount >= 60 ){ //Goを表示
		Framework::instance().drawDebugString( 0, 1, "GO!" );
	}else{ //Ready...を表示
		Framework::instance().drawDebugString( 0, 1, "Ready..." );
	}
	//描画
	parent->drawState();
	mImage->draw();
	//字を出す
	Framework::instance().drawDebugString( 0, 0, "[READY] : " );

	++mCount;

	return next;
}

} //namespace Game
} //namespace Sequence