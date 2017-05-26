#include "GameLib/Framework.h"

//楽譜定義
struct Note{
	int mName; //ドレミ。0が65Hzのド。24がいわゆる真ん中のド。
	int mLength; // 100分の1秒単位
	int mVolume; // 0から100
};

const int C = 24;
const int Cis = 25;
const int D = 26;
const int Dis = 27;
const int E = 28;
const int F = 29;
const int Fis = 30;
const int G = 31;
const int Gis = 32;
const int A = 33;
const int Ais = 34;
const int H = 35;

//右手楽譜
const Note gTrack1[] = {
	{ 0, 150, 0 }, //休符
	{ D, 50, 15 },

	{ G, 50, 17 },
	{ A, 50, 20 },
	{ H, 50, 22 },
	{ C+12, 50, 25 },

	{ D+12, 50, 30 },
	{ C+12, 25, 25 },
	{ H, 25, 22 },
	{ A, 50, 20 },
	{ D+12, 50, 22 },

	{ G+12, 50, 35 },
	{ Fis+12, 50, 32 },
	{ E+12, 50, 20 },
	{ D+12, 50, 28 },

	{ D+12, 12, 20 },
	{ Cis+12, 13, 20 },
	{ D+12, 12, 20 },
	{ Cis+12, 13, 20 },
	{ H, 25, 20 },
	{ Cis+12, 25, 20 },
	{ A, 25, 25 },
	{ H, 25, 15 },
	{ G, 25, 15 },
	{ A, 25, 15 },

	{ A+12, 100, 15 },
	{ A+12, 100, 15 },

	{ H+12, 100, 20 },
	{ H+12, 100, 20 },

	{ A+12, 50, 25 },
	{ H+12, 50, 25 },
	{ A+12, 50, 22 },
	{ G+12, 50, 20 },

	{ G+12, 12, 15 },
	{ Fis+12, 13, 15 },
	{ G+12, 12, 15 },
	{ Fis+12, 13, 15 },
	{ E+12, 25, 15 },
	{ Fis+12, 25, 15 },
	{ D+12, 100, 12 },
};

//左手楽譜
const Note gTrack2[] = {
	{ 0, 200, 0 }, //休符

	{ G-12, 100, 10 },
	{ G, 100, 10 },

	{ G, 50, 10 },
	{ Fis, 25, 10 },
	{ E, 25, 10 },
	{ Fis, 50, 10 },
	{ D, 50, 10 },

	{ E, 50, 10 },
	{ Fis, 50, 10 },
	{ G, 50, 10 },
	{ E, 50, 10 },

	{ A, 50, 10 },
	{ G, 50, 10 },
	{ Fis, 50, 10 },
	{ E, 50, 10 },

	{ Fis, 50, 10 },
	{ A, 50, 12 },
	{ D+12, 50, 15 },
	{ Cis+12, 50, 12 },

	{ H, 50, 15 },
	{ D+12, 50, 20 },
	{ G+12, 50, 25 },
	{ Fis+12, 50, 20 },

	{ E+12, 12, 10 },
	{ D+12, 13, 10 },
	{ E+12, 12, 10 },
	{ D+12, 13, 10 },
	{ E+12, 12, 10 },
	{ D+12, 13, 10 },
	{ E+12, 12, 10 },
	{ D+12, 13, 10 },
	{ E+12, 10, 10 },
	{ D+12, 10, 10 },
	{ E+12, 10, 10 },
	{ D+12, 10, 10 },
	{ Cis+12, 10, 10 },
	{ H, 25, 10 },
	{ Cis+12, 25, 10 },

	{ D+12, 25, 15 },
	{ A, 25, 15 },
	{ G, 25, 12 },
	{ A, 25, 12 },
	{ Fis, 100, 10 },
};

void addSound( 
short* data, //出力
int waveWidth, //波の幅=音量
int note, //音番号0が261Hzのド。12は522Hzのド。と言う感じの音番号
int sampleNumber ){ //データ数
	//周波数テーブル。2の12乗根(1.05946倍)づつされていく
	int frequencyTable[] = {
		261, 277, 293, 311, 329, 349, 370, 392, 415, 440, 466, 494,
	};
	int octave = note / 12; //オクターブを決定。この数だけ周波数2倍してやる。
	int f = frequencyTable[ note % 12 ]; //表を引いてドレミを決める

	//2オクターブ下にずらす
	octave -= 2;
	if ( octave >= 0 ){
		f <<= octave; //octaveが2なら4倍したいが、これは左シフト2だ。
	}else{
		f >>= -octave;
	}

	int m = waveWidth * f * 2 / 44100 ; //行って戻るので倍

	//以下三角波
	int period = 44100 / f; //iがいくつで巻き戻るか
	//後はデータ埋めるだけ
	for ( int i = 0; i < sampleNumber; ++i ){
		int j = i % period;
		int w = ( j < period / 2 ) ? ( j*m ) : ( 2*waveWidth - j*m );
		w += data[ i ];
		data[ i ] = static_cast< short >( w ); 
	}
}

bool gFirstFrame = true;

namespace GameLib{
	void Framework::update(){
		sleep( 16 );
		if ( gFirstFrame ){ //最初のフレームしか処理しない
			gFirstFrame = false;
			short* wave = new short[ 44100 * 60 ]; //とりあえず1分。配列が大きい時はnewした方がいい。

			//一旦0を埋める
			for ( int i = 0; i < 44100 * 60; ++i ){
				wave[ i ] = 0;
			}

			//楽譜を解釈します。
			//1トラック目
			int time = 0;
			int noteNumber = sizeof( gTrack1 ) / sizeof( Note ); //これで音符の数が出ます
			for ( int i = 0; i < noteNumber; ++i ){
				const Note& n = gTrack1[ i ];
				if ( n.mVolume > 0 ){ //無音なら処理飛ばす
					addSound( 
						wave + time, 
						n.mVolume * 300,
						n.mName,
						n.mLength * 441 );
				}
				time += n.mLength * 441;
			}
			//2トラック目
			time = 0;
			noteNumber = sizeof( gTrack2 ) / sizeof( Note ); //これで音符の数が出ます
			for ( int i = 0; i < noteNumber; ++i ){
				const Note& n = gTrack2[ i ];
				if ( n.mVolume > 0 ){ //無音なら処理飛ばす
					addSound( 
						wave + time, 
						n.mVolume * 300,
						n.mName,
						n.mLength * 441 );
				}
				time += n.mLength * 441;
			}
			playSound( wave, 44100 * 60 );
			SAFE_DELETE_ARRAY( wave );
		}
	}
}