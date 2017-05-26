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

const Note gNotes[] = {
	{ E, 25, 20 },
	{ Dis, 25, 20 },

	{ E, 25, 20 },
	{ Dis, 25, 20 },
	{ E, 25, 20 },
	{ H-12, 25, 20 },
	{ D, 25, 20 },
	{ C, 25, 20 },

	{ A-12, 75, 20 },
	{ C-12, 25, 20 },
	{ E-12, 25, 20 },
	{ A-12, 25, 20 },

	{ H-12, 75, 20 },
	{ E-12, 25, 20 },
	{ Gis-12, 25, 20 },
	{ H-12, 25, 20 },

	{ C, 75, 20 },
};
const int gNoteNumber = 17;

void createSound( 
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

	//iに掛ける乗数を計算しよう
	/*
	1 欲しい周波数 = 44100 / (iがいくつで波が巻き戻るか)
	2 (iがいくつで波が巻き戻るか) = 波の幅 / iに掛ける乗数
	
	1,2の式から、

	iに掛ける乗数 = 波の幅 * 欲しい周波数 / 44100
	*/

	//iに掛ける数に直す(上の式参照)
	int m = waveWidth * f / 44100;

	//後はデータ埋めるだけ
	for ( int i = 0; i < sampleNumber; ++i ){
		data[ i ] = static_cast< short >( (i*m) % waveWidth );
	}
}

bool gFirstFrame = true;

namespace GameLib{
	void Framework::update(){
		sleep( 16 );
		if ( gFirstFrame ){ //最初のフレームしか処理しない
			gFirstFrame = false;
			short* wave = new short[ 44100 * 60 ]; //とりあえず1分。配列が大きい時はnewした方がいい。

			//楽譜を解釈します。
			int time = 0;
			for ( int i = 0; i < gNoteNumber; ++i ){
				const Note& n = gNotes[ i ];
				createSound( 
					wave + time, 
					n.mVolume * 300,
					n.mName,
					n.mLength * 441 );
				time += n.mLength * 441;
			}
			//残った時間0を埋める
			for ( int i = time; i < 44100 * 60; ++i ){
				wave[ i ] = 0;
			}
			playSound( wave, 44100 * 60 );
			SAFE_DELETE_ARRAY( wave );
		}
	}
}