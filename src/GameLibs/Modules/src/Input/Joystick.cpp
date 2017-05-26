#include "GameLib/GameLib.h"
#include "GameLib/Input/Joystick.h"

#include "Input/JoystickImpl.h"

namespace GameLib{
namespace Input{

Joystick::Joystick() : mImpl( 0 ){
}

Joystick::Joystick( Impl* impl ) : mImpl( impl ){
}

Joystick::~Joystick(){
	mImpl = 0;
}

bool Joystick::isOn( int i ) const {
	return mImpl->isOn( i );
}

bool Joystick::isTriggered( int i ) const {
	return mImpl->isTriggered( i );
}

int Joystick::analogNumber() const {
	return mImpl->analogNumber();
}

int Joystick::analog( int i ) const {
	return mImpl->analog( i );
}

int Joystick::buttonNumber() const {
	return mImpl->buttonNumber();
}

bool Joystick::isEnabled() const {
	return mImpl->mEnabled;
}

Joystick::operator void*() const {
	return ( mImpl == 0 ) ? 0 : reinterpret_cast< void* >( ~0 );
}



} //namespace Input
} //namespace GameLib
