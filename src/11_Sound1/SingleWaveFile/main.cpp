#include "GameLib/Framework.h"
#include "GameLib/Sound/Wave.h"
#include "GameLib/Sound/Player.h"
using namespace GameLib::Sound;

Wave gWave;
Player gPlayer;

namespace GameLib{
	void Framework::update(){
		sleep( 16 );
		//gWaveが空ならロード開始
		if ( !gWave ){
			gWave = Sound::Wave::create( "dokaan.wav" );
		}
		if ( !gPlayer && gWave.isReady() ){ //プレイヤーが空でロードが終わっているなら
			if ( !gWave.isError() ){ //エラーかチェックしようね。ファイルを読み損ねても止まらないので。
				gPlayer = Sound::Player::create( gWave );
				gPlayer.play();
			}
		}
		if ( gPlayer ){ //gPlayerが作られているなら
			static int a = 0;
			static int b = 1;
			//ボリュームいじってみる実験
			gPlayer.setVolume( a );
			if ( a == 0 || a == -100 ){
				b *= -1;
			}
			a += b;
			if ( !gPlayer.isPlaying() ){ //終わったので解放。
				gPlayer.release();
				gWave.release();
			}
		}
		if ( isEndRequested() ){
			gWave.release();
			gPlayer.release();
		}
	}
}
