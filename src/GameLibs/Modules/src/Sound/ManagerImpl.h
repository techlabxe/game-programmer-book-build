#ifndef INCLUDED_GAMELIB_SOUND_MANAGERIMPL_H
#define INCLUDED_GAMELIB_SOUND_MANAGERIMPL_H

#include "GameLib/GameLib.h"
#include "GameLib/Sound/Manager.h"
#include "GameLib/WindowCreator/WindowCreator.h"

#include <dsound.h>

namespace GameLib{
namespace Sound{

class ManagerImpl{
public:
	ManagerImpl( HWND windowHandle ){
		HRESULT hr;
		hr = DirectSoundCreate8( NULL, &mDirectSound, NULL );
		STRONG_ASSERT( SUCCEEDED( hr ) );
		hr = mDirectSound->SetCooperativeLevel( windowHandle, DSSCL_PRIORITY );
		STRONG_ASSERT( SUCCEEDED( hr ) );
		DSBUFFERDESC desc;
		ZeroMemory( &desc, sizeof( DSBUFFERDESC ) );
		desc.dwSize = sizeof( DSBUFFERDESC );
		desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER;

		//プライマリバッファフォーマット変更
		hr = mDirectSound->CreateSoundBuffer( &desc, &mPrimaryBuffer, NULL );
		STRONG_ASSERT( SUCCEEDED( hr ) );
		WAVEFORMATEX format;
		ZeroMemory( &format, sizeof( PCMWAVEFORMAT ) );
		format.wBitsPerSample = 16;
		format.wFormatTag = WAVE_FORMAT_PCM;
		format.nChannels = 1; //モノラルでいいや。
		format.nSamplesPerSec = 44100;
		format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
		format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
		mPrimaryBuffer->SetFormat( &format );
	}
	~ManagerImpl(){
		mPrimaryBuffer->Release();
		mPrimaryBuffer = 0;
		mDirectSound->Release();
		mDirectSound = 0;
	}
	IDirectSound8* directSound(){
		return mDirectSound;
	}
	IDirectSound8* mDirectSound;
	IDirectSoundBuffer* mPrimaryBuffer;
};
extern ManagerImpl* gManagerImpl;

} //namespace Sound
} //namespace GameLib
 
#endif
