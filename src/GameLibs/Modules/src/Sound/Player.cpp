#include "GameLib/GameLib.h"
#include "GameLib/Sound/Player.h"
#include "GameLib/Sound/Wave.h"
#include "Sound/PlayerImpl.h"
#include "Sound/ManagerImpl.h"

namespace GameLib{
namespace Sound{

Player Player::create( Wave wave ){
	STRONG_ASSERT( wave.isReady() && "Player::Player() : loading hasn't finished." );
	Player r;
	r.mImpl = NEW Impl( wave.mImpl, gManagerImpl->directSound() );
	return r;
}

Player Player::create( int bits, int size, int freq, int channelN ){
	STRONG_ASSERT( bits == 8 || bits == 16 && "Player::Player() : bitsPerSample must be 8 or 16." );
	STRONG_ASSERT( ( ( bits * freq * channelN / 8 / 16 ) < size ) && "Player::Player() : bufferSize is too small. at least 1/16s" );
	Player r;
	r.mImpl = NEW Impl( bits, size, freq, channelN, gManagerImpl->directSound() );
	return r;
}

void Player::play( bool looping ){
	ASSERT( mImpl && "Sound::Player : This is empty object." );
	return mImpl->play( looping );
}

void Player::stop(){
	ASSERT( mImpl && "Sound::Player : This is empty object." );
	return mImpl->stop();
}

void Player::setVolume( int v ){
	ASSERT( mImpl && "Sound::Player : This is empty object." );
	return mImpl->setVolume( v );
}

bool Player::isPlaying() const {
	ASSERT( mImpl && "Sound::Player : This is empty object." );
	return mImpl->isPlaying();
}

bool Player::write( int pos, const char* data, int size ){
	if ( size == 0 ){ //‚â‚é‚±‚Æ‚È‚µ
		return true;
	}
	ASSERT( mImpl && "Sound::Player : This is empty object." );
	return mImpl->write( pos, data, size );
}

bool Player::fillSilence( int pos, int size ){
	if ( size == 0 ){ //‚â‚é‚±‚Æ‚È‚µ
		return true;
	}
	ASSERT( mImpl && "Sound::Player : This is empty object." );
	return mImpl->fillSilence( pos, size );
}

int Player::position() const {
	return mImpl->position();
}

#define TYPE Player
#include "GameLib/Base/Impl/ReferenceTypeTemplate.h"

} //namespace Sound
} //namespace GameLib
