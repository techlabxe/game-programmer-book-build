#include "GameLib/GameLib.h"
#include "GameLib/Input/Keyboard.h"

#include "Input/ManagerImpl.h"

//キーボードの実体はマネージャである。
namespace GameLib{
namespace Input{

bool Keyboard::isOn( int i ) const {
	return gManagerImpl->isKeyOn( i );
}

bool Keyboard::isTriggered( int i ) const {
	return gManagerImpl->isKeyTriggered( i );
}

} //namespace Input
} //namespace GameLib
