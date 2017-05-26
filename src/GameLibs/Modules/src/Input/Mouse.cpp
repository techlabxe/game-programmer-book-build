#include "GameLib/GameLib.h"
#include "GameLib/Input/Mouse.h"
#include "GameLib/WindowCreator/WindowCreator.h"

#include "Input/ManagerImpl.h"

//マウスの実体はマネージャである。
namespace GameLib{
namespace Input{

bool Mouse::isOn( Button i ) const {
	return gManagerImpl->isButtonOn( i );
}

bool Mouse::isTriggered( Button i ) const {
	return gManagerImpl->isButtonTriggered( i );
}

int Mouse::wheel() const {
	return gManagerImpl->mWheel;
}

int Mouse::x() const {
	return gManagerImpl->mX;
}

int Mouse::y() const {
	return gManagerImpl->mY;
}

int Mouse::velocityX() const {
	return gManagerImpl->mVelocityX;
}

int Mouse::velocityY() const {
	return gManagerImpl->mVelocityY;
}

} //namespace Input
} //namespace GameLib
