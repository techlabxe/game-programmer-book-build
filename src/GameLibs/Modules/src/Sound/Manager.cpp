#include "GameLib/GameLib.h"
#include "GameLib/Sound/Manager.h"

#include "Sound/WaveImpl.h"
#include "Sound/PlayerImpl.h"
#include "Sound/ManagerImpl.h"
#include "GameLib/WindowCreator/WindowCreator.h"

#include <dsound.h>

namespace GameLib{
namespace Sound{

ManagerImpl* gManagerImpl = 0; //Implの実体

Manager::Manager(){
	//別スレッドからの呼び出しは許さない
	ASSERT( WindowCreator::isMainThread() && "you must call from MAIN thread" );
}

void Manager::create( void* windowHandle ){
	STRONG_ASSERT( !gManagerImpl );
	gManagerImpl = NEW ManagerImpl( static_cast< HWND >( windowHandle ) );
}

void Manager::destroy(){
	SAFE_DELETE( gManagerImpl );
}

Manager Manager::instance(){
	return Manager();
}

} //namespace Sound
} //namespace GameLib
 