#ifndef INCLUDED_SOUND_MANAGER_H
#define INCLUDED_SOUND_MANAGER_H

#include "GameLib/Sound/Wave.h"
#include "GameLib/Sound/Player.h"

//シングルトン
class SoundManager{
public:
	static SoundManager* instance();
	static void create();
	static void destroy();
	bool isReady(); //ロード終わった？

	enum Bgm{
		BGM_TITLE, //タイトル画面
		BGM_GAME, //ゲーム中

		BGM_MAX,
	};
	enum Se{
		SE_EXPLOSION, //爆発
		SE_DEATH, //死亡
		SE_CURSOR_MOVE, //カーソル移動
		SE_SELECTION, //項目決定
		SE_SET_BOMB, //爆弾設置音

		SE_MAX,
	};
	//BGMを鳴らす。BGMは同時に一個しか鳴らない。また、勝手にループする。
	void playBgm( Bgm );
	void stopBgm();
	//SEを鳴らす。SEは鳴らせといわれただけ鳴る。
	void playSe( Se );
private:
	SoundManager();
	~SoundManager();

	//Waveは前もってロードしてしまう。
	GameLib::Sound::Wave mBgmWaves[ BGM_MAX ];
	GameLib::Sound::Wave mSeWaves[ SE_MAX ];
	//Playerは再生時生成
	GameLib::Sound::Player mBgmPlayer; //一個だけ作っておく
	static const int SE_PLAYER_MAX = 4; //同時に鳴らすSEは4つまで。古い順に消していく。
	GameLib::Sound::Player mSePlayers[ SE_PLAYER_MAX ]; //これは配列で毎フレームチェックして終わってたら削除。
	int mSePlayerPos; //次にSEを入れる場所。

	static SoundManager* mInstance;
};

#endif
