#ifndef INCLUDED_GAMELIB_INPUT_JOYSTICKIMPL_H
#define INCLUDED_GAMELIB_INPUT_JOYSTICKIMPL_H

#include <dinput.h>
#undef min
#undef max

namespace GameLib{
namespace Input{

class Joystick::Impl{
public:
	Impl( IDirectInputDevice8* device, HWND windowHandle ) : 
	mDevice( device ),
	mDoubleBufferIndex( 0 ),
	mEnabled( false ){
		HRESULT hr;
		hr = mDevice->SetDataFormat( &c_dfDIJoystick2 );
		STRONG_ASSERT( SUCCEEDED( hr ) );
		hr = mDevice->SetCooperativeLevel( windowHandle, DISCL_FOREGROUND | DISCL_EXCLUSIVE );
		STRONG_ASSERT( SUCCEEDED( hr ) );
		for ( int i = 0; i < BUTTON_MAX; ++i ){
			mButtons[ 0 ][ i ] = false;
			mButtons[ 1 ][ i ] = false;
		}
		for ( int i = 0; i < ANALOG_MAX; ++i ){
			mAnalogs[ i ] = 0;
		}

	}
	~Impl(){
		mDevice->Unacquire();
		mDevice->Release();
		mDevice = 0;
	}
	void update(){
		mDoubleBufferIndex = 1 - mDoubleBufferIndex;

		HRESULT hr = mDevice->Acquire();
		STRONG_ASSERT( hr != DIERR_INVALIDPARAM && "Keyboard::Impl::update() : DI_INVALIDPARAM" );
		STRONG_ASSERT( hr != DIERR_NOTINITIALIZED && "Keyboard::Impl::update() : DI_NOTINITIALIZED" );
		DIJOYSTATE2 state;
		hr = mDevice->GetDeviceState( sizeof( DIJOYSTATE2 ), &state );
		if ( SUCCEEDED( hr ) ){
			for ( int i = 0; i < BUTTON_MAX; ++i ){
				mButtons[ mDoubleBufferIndex ][ i ] = state.rgbButtons[ i ] ? true : false;
			}
			mAnalogs[ 0 ] = state.lX;
			mAnalogs[ 1 ] = state.lY;
			mAnalogs[ 2 ] = state.lZ;
			mAnalogs[ 3 ] = state.lRx;
			mAnalogs[ 4 ] = state.lRy;
			mAnalogs[ 5 ] = state.lRz;
			mAnalogs[ 6 ] = state.rglSlider[ 0 ];
			mAnalogs[ 7 ] = state.rglSlider[ 1 ];
			int pov = state.rgdwPOV[ 0 ];

			int dx = 0;
			int dy = 0;
			if ( ( pov & 0xffff ) != 0xffff ){
				//8•ûŒü”»’è
				if ( pov > 36000*15/16 ){
					dx = 0;
				}else if ( pov > 36000*9/16 ){
					dx = -1;
				}else if ( pov > 36000*7/16 ){
					dx = 0;
				}else if ( pov > 36000*1/16 ){
					dx = 1;
				}else{
					dx = 0;
				}
				if ( pov > 36000*13/16 ){
					dy = 1;
				}else if ( pov > 36000*11/16){
					dy = 0;
				}else if ( pov > 36000*5/16){
					dy = -1;
				}else if ( pov > 36000*3/16){
					dy = 0;
				}else{
					dy = 1;
				}
			}
			if ( dx > 0 ){
				mButtons[ mDoubleBufferIndex ][ Joystick::BUTTON_RIGHT ] = true;
				mButtons[ mDoubleBufferIndex ][ Joystick::BUTTON_LEFT ] = false;
			}else if ( dx < 0 ){
				mButtons[ mDoubleBufferIndex ][ Joystick::BUTTON_RIGHT ] = false;
				mButtons[ mDoubleBufferIndex ][ Joystick::BUTTON_LEFT ] = true;
			}
			if ( dy > 0 ){
				mButtons[ mDoubleBufferIndex ][ Joystick::BUTTON_UP ] = true;
				mButtons[ mDoubleBufferIndex ][ Joystick::BUTTON_DOWN ] = false;
			}else if ( dy < 0 ){
				mButtons[ mDoubleBufferIndex ][ Joystick::BUTTON_UP ] = false;
				mButtons[ mDoubleBufferIndex ][ Joystick::BUTTON_DOWN ] = true;
			}
			mEnabled = true;
		}else{
			for ( int i = 0; i < BUTTON_MAX; ++i ){
				mButtons[ mDoubleBufferIndex ][ i ] = false;
			}
			for ( int i = 0; i < ANALOG_MAX; ++i ){
				mAnalogs[ i ] = 0;
			}
			mEnabled = false;
		}
	}
	bool isOn( int i ) const {
		ASSERT( i < BUTTON_MAX );
		return mButtons[ mDoubleBufferIndex ][ i ];
	}
	bool isTriggered( int i ) const {
		ASSERT( i < BUTTON_MAX );
		int idx = mDoubleBufferIndex;
		return mButtons[ idx ][ i ] && ( !mButtons[ 1 - idx ][ i ] );
	}
	int analog( int i ) const {
		return mAnalogs[ i ];
	}
	int analogNumber() const {
		return ANALOG_MAX;
	}
	int buttonNumber() const {
		return BUTTON_MAX - 4; //ã‰º¶‰E‚ÍœŠO
	}
	IDirectInputDevice8* mDevice;
	static const int BUTTON_MAX = 128 + 4; //ã‰º¶‰E•ª
	static const int ANALOG_MAX = 8;
	bool mButtons[ 2 ][ BUTTON_MAX ];
	int mAnalogs[ 8 ];
	int mDoubleBufferIndex;
	bool mEnabled;
};

} //namespace Input
} //namespace GameLib

#endif