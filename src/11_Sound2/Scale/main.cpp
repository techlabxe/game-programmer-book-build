#include "GameLib/Framework.h"

void createSound( 
short* data, //出力
int waveWidth, //波の幅=音量
int note, //音番号0が262Hzのド。12は522Hzのド。と言う感じの音番号
int sampleNumber ){ //データ数
	//周波数テーブル。2の12乗根(1.05946倍)づつされていく
	int frequencyTable[] = {
		261, 277, 293, 311, 329, 349, 370, 392, 415, 440, 466, 494,
	};
	int octave = note / 12; //オクターブを決定。この数だけ周波数2倍してやる。
	int f = frequencyTable[ note % 12 ]; //表を引いてドレミを決める
	f <<= octave; //octaveが2なら4倍したいが、これは左シフト2だ。

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

namespace GameLib{
	void Framework::update(){
		sleep( 16 );
		static int a = 0;
		static int b = 0;
		if ( a++ % 120 == 0 ){
			short wave[ 44100 ];
			//ドミソミドー、みたいな感じで5個。音楽の授業を思い出しませんか？
			createSound( wave + 44100*0/5, 20000, b+0, 44100/5 );
			createSound( wave + 44100*1/5, 20000, b+4, 44100/5 );
			createSound( wave + 44100*2/5, 20000, b+7, 44100/5 );
			createSound( wave + 44100*3/5, 20000, b+4, 44100/5 );
			createSound( wave + 44100*4/5, 20000, b+0, 44100/5 );

			playSound( wave, 44100 );
			++b;
		}
	}
}