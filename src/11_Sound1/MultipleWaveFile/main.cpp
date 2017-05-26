#include "GameLib/Framework.h"
#include "GameLib/Sound/Wave.h"
#include "GameLib/Sound/Player.h"
using namespace GameLib::Sound;

Wave gWaveSE;
Wave gWaveBGM;
Player gPlayerSE0;
Player gPlayerSE1;
Player gPlayerBGM;

#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include <sstream>
using namespace std;

namespace GameLib{
	void Framework::update(){
		sleep( 16 );
		//SEロード
		if ( !gWaveSE ){
			gWaveSE = Sound::Wave::create( "dokaan.wav" );
		}
		//BGMロード
		if ( !gWaveBGM ){
			gWaveBGM = Sound::Wave::create( "charara.wav" );
		}
		//SEプレイヤー作成
		if ( !gPlayerSE0 && !gPlayerSE1 && gWaveSE.isReady() ){ //プレイヤーが空でロードが終わっているなら
			if ( !gWaveSE.isError() ){ //エラーかチェックしようね。ファイルを読み損ねても止まらないので。
				//1Waveから2個プレイヤー作成
				gPlayerSE0 = Sound::Player::create( gWaveSE );
				gPlayerSE1 = Sound::Player::create( gWaveSE );
			}
		}
		//BGMプレイヤー作成
		if ( !gPlayerBGM && gWaveBGM.isReady() ){ //プレイヤーが空でロードが終わっているなら
			if ( !gWaveBGM.isError() ){ //エラーかチェックしようね。ファイルを読み損ねても止まらないので。
				gPlayerBGM = Sound::Player::create( gWaveBGM );
				gPlayerBGM.play( true ); //ループ再生。無限に鳴る。
			}
		}
		//SEは80*16ミリ秒ごとに交互に鳴らす。片方は小さく。
		if ( gPlayerSE0 && gPlayerSE1 ){
			static int a;
			if ( a % 80 == 0 ){
				gPlayerSE0.setVolume( -10 );
				gPlayerSE0.play(); 
			}
			if ( a % 80 == 40 ){
				gPlayerSE1.setVolume( -20 );
				gPlayerSE1.play(); 
			}
			++a;
		}

		static int v = 0;
		if ( Input::Manager::instance().keyboard().isTriggered( Input::Keyboard::KEY_DOWN ) ){
			--v;
			if ( v < -100 ){
				v = -100;
			}
		}
		if ( Input::Manager::instance().keyboard().isTriggered( Input::Keyboard::KEY_UP ) ){
			++v;
			if ( v > 0 ){
				v = 0;
			}
		}
		ostringstream oss;
		oss << "VOLUME: " << v;
		drawDebugString( 0, 0, oss.str().c_str() );
		if ( gPlayerBGM ){
			gPlayerBGM.setVolume( v );
		}
		if ( isEndRequested() ){
			gWaveSE.release();
			gWaveBGM.release();
			gPlayerSE0.release();
			gPlayerSE1.release();
			gPlayerBGM.release();
		}
	}
}
