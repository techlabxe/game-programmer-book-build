#include "GameLib/GameLib.h"
#include "Track.h"
#include "Note.h"
#include <cmath>
using namespace std;

Track::Track() : mNotes( 0 ), mNoteNumber( 0 ){}

Track::~Track(){
	if ( mNotes ){
		SAFE_DELETE_ARRAY( mNotes );
	}
	mNoteNumber = 0;
}

void Track::setData( const Note* notes, int noteNumber ){
	mNotes = new Note[ noteNumber ];
	for ( int i = 0; i < noteNumber; ++i ){
		mNotes[ i ] = notes[ i ];
	}
	mNoteNumber = noteNumber;
}

void Track::addWave( short* buffer, int bufferSize, int frequency ) const {
	int time = 0; //1/100単位時間
	int pos = 0; //サンプル数単位時間
	for ( int i = 0; i < mNoteNumber; ++i ){
		const Note& n = mNotes[ i ];
		int endTime = time + n.mLength;
		int length = ( endTime * frequency / 100 ) - pos;
		if ( pos + length >= bufferSize ){
			length = bufferSize - pos; //あふれてます
		}
		if ( n.mVolume > 0 ){ //無音なら処理飛ばす
			addNote( 
				buffer + pos,
				n.mVolume * 32767 / 100,
				n.mName,
				length,
				frequency );
		}
		time = endTime;
		pos += length;
	}
}

void Track::addNote( 
short* buffer,
int waveWidth, 
int name, 
int sampleNumber,
int frequency ) const {
	//57が440Hzになるようにnameを解釈する。
	double f = 440.0 * pow( 2.0, ( name - 57 ) * ( 1.0 / 12.0 ) );

	//何かと使うPI
	const double pi = 3.1415926535897932384626433832795;
	//以下正弦波合成
	/*
	y = c*sin( ax )
	が正弦波の式。sineは2PIで1周期だから、まずa=2PIにするとxが1で1周期回る。
	実際の周期はf/44100だからこれを乗じて
	a = 2PI * f / 44100;
	bはwaveWidthの半分だ。-bからbまで動く。
	このサンプルではだんだん音が小さくなるようにしてみた。
	*/
	double a = f * ( 2.0 * pi ) / frequency;
	double b = waveWidth / 2.0;
	double rcpSampleNumber = 1.0 / sampleNumber;

	//楽しい音色合成(トランペットのフーリエ解析結果(7倍音まで))
	//ここをいじるといろんな音色ができます。
	//各倍音の含有量
	const double factor[] = { 
		0.5, //基底周波数
		0.85, //2倍音
		1.05, //3倍音
		0.85,
		0.4,
		0.3,
		0.1,
		0.0
	};
	//各倍音の位相(単位は度)
	const double phase[] = { 
		339.0, //基底周波数
		9.0, //2倍音
		18.0, //3倍音
		3.0,
		351.0,
		353.0,
		358.0,
		0,
	};
	//ビブラート用sin(ax)のa(秒間8回)
	const double va = 8.f * ( 2.0 * pi ) / frequency;

	for ( int i = 0; i < sampleNumber; ++i ){
		double posInNote = i * rcpSampleNumber; //音符の中の位置
		double tb = b * ( 1.0 - posInNote ); //音量をだんだん小さく
		tb *= ( 1.0 + posInNote * 0.2 * sin( va * i ) ); //ヴィブラート(音符の後ろほど強く)
		//8個の正弦波を合成
		double w = 0.0;
		for ( int j = 0; j < 8; ++j ){
			w += tb * sin( a * j * i + ( phase[ j ] / pi ) ) * factor[ j ];
		}
		w += buffer[ i ];
 		buffer[ i ] = static_cast< short >( w );
	}
}
