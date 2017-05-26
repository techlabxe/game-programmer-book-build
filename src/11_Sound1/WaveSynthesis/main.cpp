#include "GameLib/Framework.h"
#include "GameLib/Sound/Wave.h"
#include "GameLib/Sound/Player.h"
using namespace GameLib::Sound;

#include "Note.h"
#include "Track.h"

const int C = 48;
const int Cis = 49;
const int D = 50;
const int Dis = 51;
const int E = 52;
const int F = 53;
const int Fis = 54;
const int G = 55;
const int Gis = 56;
const int A = 57;
const int Ais = 58;
const int H = 59;

//右手楽譜
const Note gTrack0Notes[] = {
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
const int gTrack0NoteNumber = sizeof( gTrack0Notes ) / sizeof( Note );

//左手楽譜
const Note gTrack1Notes[] = {
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
const int gTrack1NoteNumber = sizeof( gTrack1Notes ) / sizeof( Note );

Wave gWave;
Player gPlayer;

const int FREQUENCY = 44100;
const int TIME = 20; //20秒
const int BUFFER_SIZE = FREQUENCY * sizeof( short ) * TIME;

namespace GameLib{
	void Framework::update(){
		sleep( 16 );
		if ( !gWave ){
			//トラッククラス生成
			Track tracks[ 2 ];
			tracks[ 0 ].setData( gTrack0Notes, gTrack0NoteNumber );
			tracks[ 1 ].setData( gTrack1Notes, gTrack1NoteNumber );
			short* buffer = new short[ BUFFER_SIZE ];
			for ( int i = 0; i < BUFFER_SIZE; ++i ){
				buffer[ i ] = 0;
			}
			for ( int i = 0; i < 2; ++i ){
				tracks[ i ].addWave( buffer, BUFFER_SIZE, FREQUENCY );
			}
			gWave = Sound::Wave::create( buffer, BUFFER_SIZE, FREQUENCY, 1 );
			SAFE_DELETE_ARRAY( buffer );
			gPlayer = Sound::Player::create( gWave );
			gPlayer.play();
		}
		if ( isEndRequested() ){
			//破棄します。
			gPlayer.release();
			gWave.release();
		}
	}
}


