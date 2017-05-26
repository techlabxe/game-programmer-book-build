#ifndef INCLUDED_GAMELIB_SOUND_PLAYERIMPL_H
#define INCLUDED_GAMELIB_SOUND_PLAYERIMPL_H

#include "GameLib/GameLib.h"
#include "GameLib/Sound/Player.h"
#include "GameLib/Base/Impl/ReferenceType.h"
#include "Sound/WaveImpl.h"

namespace GameLib{
namespace Sound{

class Player::Impl : public ReferenceType{
public:
	Impl( Wave::Impl* wave, IDirectSound8* directSound ) : 
	mBuffer( 0 ),
	mWave( wave ),
	mIs8Bit( false ),
	mSize ( 0 ){
		mWave->refer();
		HRESULT hr;
		IDirectSoundBuffer* buffer;
		hr = directSound->DuplicateSoundBuffer( wave->buffer(), &buffer );
		STRONG_ASSERT( hr != DSERR_ALLOCATED && "Sound::Player::Impl::Impl : DSERR_ALLOCATED" );
		STRONG_ASSERT( hr != DSERR_INVALIDCALL && "Sound::Player::Impl::Impl : DSERR_INVALIDCALL" );
		STRONG_ASSERT( hr != DSERR_INVALIDPARAM && "Sound::Player::Impl::Impl : DSERR_INVALIDPARAM" );
		STRONG_ASSERT( hr != DSERR_OUTOFMEMORY && "Sound::Player::Impl::Impl : DSERR_OUTOFMEMORY" );
		STRONG_ASSERT( hr != DSERR_UNINITIALIZED && "Sound::Player::Impl::Impl : DSERR_UNINITIALIZED" );

		hr = buffer->QueryInterface( IID_IDirectSoundBuffer8, ( void** )&mBuffer );
		STRONG_ASSERT( SUCCEEDED( hr ) );
		buffer->Release();
		buffer = 0;
	}
	Impl( int bit, int size, int freq, int channelN, IDirectSound8* directSound ) :
	mBuffer( 0 ),
	mWave( 0 ),
	mSize( size ){
		mIs8Bit = ( bit == 8 );

		WAVEFORMATEX format;
		ZeroMemory( &format, sizeof( WAVEFORMATEX ) );
		format.wFormatTag = WAVE_FORMAT_PCM;
		format.nChannels = static_cast< WORD >( channelN );
		format.nSamplesPerSec = freq;
		format.wBitsPerSample = static_cast< WORD >( bit );
		format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
		format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

		DSBUFFERDESC desc;
		ZeroMemory( &desc, sizeof( DSBUFFERDESC ) );
		desc.dwSize = sizeof( DSBUFFERDESC );
		desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
		desc.dwBufferBytes = size;
		desc.lpwfxFormat = &format;

		IDirectSoundBuffer* buffer = 0;
		HRESULT hr = directSound->CreateSoundBuffer( &desc, &buffer, NULL );
		STRONG_ASSERT( hr != DSERR_ALLOCATED && "Sound::Wave::Impl::Impl : DSERR_ALLOCATED" );
		STRONG_ASSERT( hr != DSERR_BADFORMAT && "Sound::Wave::Impl::Impl : DSERR_BADFORMAT" );
		STRONG_ASSERT( hr != DSERR_BUFFERTOOSMALL && "Sound::Wave::Impl::Impl : DSERR_BUFFERTOOSMALL" );
		STRONG_ASSERT( hr != DSERR_CONTROLUNAVAIL && "Sound::Wave::Impl::Impl : DSERR_CONTROLUNAVAIL" );
		STRONG_ASSERT( hr != DSERR_DS8_REQUIRED && "Sound::Wave::Impl::Impl : DSERR_DS8_REQUIRED" );
		STRONG_ASSERT( hr != DSERR_INVALIDCALL && "Sound::Wave::Impl::Impl : DSERR_INVALIDCALL" );
		STRONG_ASSERT( hr != DSERR_INVALIDPARAM && "Sound::Wave::Impl::Impl : DSERR_INVALIDPARAM" );
		STRONG_ASSERT( hr != DSERR_NOAGGREGATION && "Sound::Wave::Impl::Impl : DSERR_NOAGGREGATION" );
		STRONG_ASSERT( hr != DSERR_OUTOFMEMORY && "Sound::Wave::Impl::Impl : DSERR_OUTOFMEMORY" );
		STRONG_ASSERT( hr != DSERR_UNINITIALIZED && "Sound::Wave::Impl::Impl : DSERR_UNINITIALIZED" );
		STRONG_ASSERT( hr != DSERR_UNSUPPORTED && "Sound::Wave::Impl::Impl : DSERR_UNSUPPORTED" );

		hr = buffer->QueryInterface( IID_IDirectSoundBuffer8, ( void** )&mBuffer );
		STRONG_ASSERT( SUCCEEDED( hr ) );
		buffer->Release();
		buffer = 0;
		//無音をつめる
		fillSilence( 0, size );
	}
	~Impl(){
		if ( mBuffer ){
			mBuffer->Stop();
			mBuffer->Release();
			mBuffer = 0;
		}
		if ( mWave ){
			mWave->release();
			if ( mWave->referenceCount() == 0 ){
				SAFE_DELETE( mWave );
			}
			mWave = 0;
		}
	}
	void play( bool looping ){
		if ( mSize > 0 ){ //ストリーミングバッファなので常にループ
			looping = true;
		}
		rewind();
		DWORD loopFlag = ( looping ) ? DSBPLAY_LOOPING : 0;
		HRESULT hr = mBuffer->Play( 0, 0, loopFlag );
		ASSERT( hr != DSERR_BUFFERLOST && "Sound::Player::Impl::play : DSERR_BUFFERLOST" );
		ASSERT( hr != DSERR_INVALIDCALL && "Sound::Player::Impl::play : DSERR_INVALIDCALL" );
		ASSERT( hr != DSERR_INVALIDPARAM && "Sound::Player::Impl::play : DSERR_INVALIDPARAM" );
		ASSERT( hr != DSERR_PRIOLEVELNEEDED && "Sound::Player::Impl::play : DSERR_PRIOLEVELNEEDED" );
	}
	void stop(){
		mBuffer->Stop();
		rewind();
	}
	void rewind(){
		HRESULT hr = mBuffer->SetCurrentPosition( 0 );
		ASSERT( hr != DSERR_INVALIDCALL && "Sound::Player::Impl::rewind : DSERR_INVALIDCALL" );
		ASSERT( hr != DSERR_INVALIDPARAM && "Sound::Player::Impl::rewind : DSERR_INVALIDPARAM" );
		ASSERT( hr != DSERR_PRIOLEVELNEEDED && "Sound::Player::Impl::rewind : DSERR_PRIOLEVELNEEDED" );
	}
	void setVolume( int volume ){
		ASSERT( volume <= 0 && volume >= -100 && "Sound::Player::setVolume : value must be between 0 and -100" ); 
		//0-100を10000-0に変換する
		LONG v = DSBVOLUME_MAX + ( DSBVOLUME_MAX - DSBVOLUME_MIN ) * volume / 100;
		HRESULT hr = mBuffer->SetVolume( v );
		ASSERT( hr != DSERR_CONTROLUNAVAIL && "Sound::Player::Impl::setVolume : DSERR_CONTROLUNAVAIL" );
		ASSERT( hr != DSERR_GENERIC && "Sound::Player::Impl::setVolume : DSERR_GENERIC" );
		ASSERT( hr != DSERR_INVALIDPARAM && "Sound::Player::Impl::setVolume : DSERR_INVALIDPARAM" );
		ASSERT( hr != DSERR_PRIOLEVELNEEDED && "Sound::Player::Impl::setVolume : DSERR_PRIOLEVELNEEDED" );
	}
	bool isPlaying() const {
		DWORD state;
		HRESULT hr = mBuffer->GetStatus( &state );
		ASSERT( hr != DSERR_INVALIDPARAM && "Sound::Player::Impl::isPlaying : DSERR_INVALIDPARAM" );
		ASSERT( !( state & DSBSTATUS_BUFFERLOST ) && "Sound::Player::Impl::isPlaying : Buffer Lost!" );
		return ( ( state & DSBSTATUS_PLAYING ) != 0 );
	}
	bool write( int pos, const char* data, int size ){
		STRONG_ASSERT( mSize > 0 && "Sound::Player::Impl::write : this isn't streaming player!" );
		//書き込んでいいのかチェック
		DWORD play, write;
		getPosition( &play, &write );
		DWORD uPos = static_cast< DWORD >( pos );

		//書き込み予定位置がplay<pos<writeになってはならない
		if ( ( play < uPos ) && ( uPos < write ) ){
			return false;
		} 

		STRONG_ASSERT( pos + size <= mSize );
		void* origP = 0;
		DWORD s;
		HRESULT hr = mBuffer->Lock( uPos, size, &origP, &s, NULL, NULL, 0 );
		ASSERT( s == static_cast< unsigned >( size ) );
		ASSERT( hr != DSERR_BUFFERLOST && "Sound::Player::Impl::write : DSERR_BUFFERLOST" );
		ASSERT( hr != DSERR_INVALIDCALL && "Sound::Player::Impl::write : DSERR_INVALIDCALL" );
		ASSERT( hr != DSERR_INVALIDPARAM && "Sound::Player::Impl::write : DSERR_INVALIDPARAM" );
		ASSERT( hr != DSERR_PRIOLEVELNEEDED && "Sound::Player::Impl::write : DSERR_PRIOLEVELNEEDED" );
		unsigned char* p = static_cast< unsigned char* >( origP );
		for ( int i = 0; i < size; ++i ){
			p[ i ] = data[ i ];
		}
		hr = mBuffer->Unlock( origP, s, NULL, NULL );
		ASSERT( hr != DSERR_INVALIDCALL && "Sound::Player::Impl::write : DSERR_INVALIDCALL" );
		ASSERT( hr != DSERR_INVALIDPARAM && "Sound::Player::Impl::write : DSERR_INVALIDPARAM" );
		ASSERT( hr != DSERR_PRIOLEVELNEEDED && "Sound::Player::Impl::write : DSERR_PRIOLEVELNEEDED" );
		return true;
	}
	bool fillSilence( int pos, int size ){
		STRONG_ASSERT( mSize > 0 && "Sound::Player::Impl::fillSilence : this isn't streaming player!" );
		//書き込んでいいのかチェック
		DWORD play, write;
		getPosition( &play, &write );
		DWORD uPos = static_cast< DWORD >( pos );

		//書き込み予定位置がplay<pos<writeになってはならない
		if ( ( play < uPos ) && ( uPos < write ) ){
			return false;
		} 

		STRONG_ASSERT( pos + size <= mSize );
		void* origP = 0;
		DWORD s;
		HRESULT hr = mBuffer->Lock( uPos, size, &origP, &s, NULL, NULL, 0 );
		ASSERT( s == static_cast< unsigned >( size ) );
		ASSERT( hr != DSERR_BUFFERLOST && "Sound::Player::Impl::fillSilence : DSERR_BUFFERLOST" );
		ASSERT( hr != DSERR_INVALIDCALL && "Sound::Player::Impl::fillSilence : DSERR_INVALIDCALL" );
		ASSERT( hr != DSERR_INVALIDPARAM && "Sound::Player::Impl::fillSilence : DSERR_INVALIDPARAM" );
		ASSERT( hr != DSERR_PRIOLEVELNEEDED && "Sound::Player::Impl::fillSilence : DSERR_PRIOLEVELNEEDED" );
		if ( mIs8Bit ){
			unsigned char* p = static_cast< unsigned char* >( origP );
			for ( int i = 0; i < size; ++i ){
				p[ i ] = 128;
			}
		}else{
			short* p = static_cast< short* >( origP );
			for ( int i = 0; i < size / 2; ++i ){
				p[ i ] = 0;
			}
		}
		hr = mBuffer->Unlock( origP, s, NULL, NULL );
		ASSERT( hr != DSERR_INVALIDCALL && "Sound::Player::Impl::fillSilence : DSERR_INVALIDCALL" );
		ASSERT( hr != DSERR_INVALIDPARAM && "Sound::Player::Impl::fillSilence : DSERR_INVALIDPARAM" );
		ASSERT( hr != DSERR_PRIOLEVELNEEDED && "Sound::Player::Impl::fillSilence : DSERR_PRIOLEVELNEEDED" );
		return true;
	}
	int position() const {
		DWORD play, write;
		getPosition( &play, &write );
		return static_cast< int >( play );
	}
	void getPosition( DWORD* play, DWORD* write ) const {
		HRESULT hr = mBuffer->GetCurrentPosition( play, write );
		ASSERT( hr != DSERR_INVALIDPARAM && "Sound::Player::Impl::getPosition : DSERR_INVALIDPARAM" );
		ASSERT( hr != DSERR_PRIOLEVELNEEDED && "Sound::Player::Impl::getPosition : DSERR_PRIOLEVELNEEDED" );
	}
private:
	IDirectSoundBuffer8* mBuffer;
	Wave::Impl* mWave;
	//以下ストリーミング用
	bool mIs8Bit;
	int mSize;

};

} //namespace Sound
} //namespace GameLib

#endif
