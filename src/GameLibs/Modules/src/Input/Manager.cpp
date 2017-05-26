#include "GameLib/GameLib.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Mouse.h"
#include "GameLib/Input/Joystick.h"

#include "Input/ManagerImpl.h"

namespace GameLib{
namespace Input{

ManagerImpl* gManagerImpl; //唯一インスタンス

Manager::Manager(){
	//別スレッドからの呼び出しは許さない
	ASSERT( WindowCreator::isMainThread() && "you must call from MAIN thread" );
}

void Manager::create( void* wh ){
	STRONG_ASSERT( !gManagerImpl );
	gManagerImpl = NEW ManagerImpl( static_cast< HWND >( wh ) );
}

void Manager::destroy(){
	SAFE_DELETE( gManagerImpl );
}

void Manager::update( float pointerScale, const Vector2& pointerOffset ){
	gManagerImpl->update( pointerScale, pointerOffset );
}

Manager Manager::instance(){
	return Manager();
}

int Manager::joystickNumber() const {
	return gManagerImpl->mJoystickNumber;
}

Keyboard Manager::keyboard() const {
	return Keyboard();
}

Mouse Manager::mouse() const {
	return Mouse();
}

Joystick Manager::joystick( int i ) const {
	ASSERT( i >= 0 && i < gManagerImpl->mJoystickNumber );
	return Joystick( &gManagerImpl->mJoysticks[ i ] );
}

} //nemespace Input
} //namespace GameLib
