#include "GameLib/GameLib.h"
#include "GameLib/Sound/Wave.h"
#include "Sound/WaveImpl.h"
#include "Sound/ManagerImpl.h"

namespace GameLib{
namespace Sound{

Wave Wave::create( const char* filename ){
	Wave r;
	r.mImpl = NEW Impl( filename, gManagerImpl->directSound() );
	return r;
}

Wave Wave::create( 
const unsigned char* data, 
int size, 
int frequency, 
int channelNumber ){
	Wave r;
	r.mImpl = NEW Impl( 
		data, 
		sizeof( unsigned char ) * 8, 
		size,
		frequency, 
		channelNumber,
		gManagerImpl->directSound() );
	return r;
}

Wave Wave::create( 
const short* data, 
int size, 
int frequency, 
int channelNumber ){
	Wave r;
	r.mImpl = NEW Impl( 
		data, 
		sizeof( short ) * 8, 
		size,
		frequency, 
		channelNumber,
		gManagerImpl->directSound() );
	return r;
}

bool Wave::isReady(){
	return ( mImpl && mImpl->isReady() );
}

bool Wave::isError() const {
	ASSERT( mImpl && "Sound::Wave : This is empty object." );
	return mImpl->isError();
}

#define TYPE Wave
#include "GameLib/Base/Impl/ReferenceTypeTemplate.h"

} //namespace Sound
} //namespace GameLib
