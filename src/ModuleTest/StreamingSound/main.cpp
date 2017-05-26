#include "GameLib/Framework.h"
#include "GameLib/Sound/Player.h"
#include "GameLib/FileIO/InFile.h"

using namespace GameLib;
using namespace GameLib::Sound;
using namespace GameLib::FileIO;

InFile gFile;
Player gPlayer;
const int BUFFER_SIZE = 16 * 1024; //これ以下はお勧めしない。1/16秒分以上ないとPlayerコンストラクタではねられる。
const int BUFFER_HALFSIZE = BUFFER_SIZE / 2;

int gReadPos = 0;
int gWritePos = 0;
int gWaveSize;
int gWaveStart;

namespace GameLib{
	void Framework::configure( Configuration* ){}
	void Framework::update(){
		if ( !gFile ){
			gFile = InFile::create( "charara.wav" );
			while ( !gFile.isFinished() ){ ; } //ロード待ち

			//--------Wavファイルを解析してみよう。

			int channels = gFile.getShort( 22 ); //22バイト目から2バイトがチャネル数
			int frequency = gFile.getInt( 24 ); //24バイト目から4バイトが周波数
			int bitsPerSample = gFile.getShort( 34 ); //34バイト目から2バイトがビット数
			const char* data = gFile.data();
			int pos = 36; //36バイト目から読んでいく
			//いらないものを読み飛ばす
			while ( data[ pos+0 ] != 'd' || data[ pos+1 ] != 'a' || data[ pos+2 ] != 't' || data[ pos+3 ] != 'a' ){ //"data"と並んでいるものが見つかるまで進める。
				pos += gFile.getInt( pos+4 ); //いらないもののサイズがここに入っている
			}
			//やっとデータが見つかった
			gWaveSize = gFile.getInt( pos + 4 ); //データの量は"data"の次から4バイト
			gWaveStart = pos + 8; //さらに次からが波
			if ( gWaveSize + gWaveStart > gFile.size() ){ //たまにサイズにウソが書いてあるファイルがある。
				gWaveSize = gFile.size() - gWaveStart; //仕方がないので末尾まで。
			}


			//プレイヤー生成
			gPlayer = Player::create( bitsPerSample, BUFFER_SIZE, frequency, channels );

			//最初の部分を充填
			int restSize = gWaveSize - gReadPos; //残りファイルサイズ
			int writeSize = ( restSize >= BUFFER_HALFSIZE ) ? BUFFER_HALFSIZE : restSize; //バッファの半分より大きければそれだけ。小さければ残り全部。
			bool succeeded = gPlayer.write( gWritePos, gFile.data() + 44 + gReadPos, writeSize ); //書き込み。
			if ( !succeeded ){
				cout << "WRITING FAILED!" << endl;
			}
			gReadPos += writeSize;
			gWritePos += writeSize;
			//足りないデータは0を埋める
			if ( writeSize < BUFFER_HALFSIZE ){
				gPlayer.fillSilence( gWritePos, BUFFER_HALFSIZE - writeSize );
				gWritePos += BUFFER_HALFSIZE - writeSize;
			}
			gPlayer.play(); //ストリーミング再生時は何も渡さなくても勝手にループ再生になる。曲そのものをループさせる処理は自前でやること。
		}
		///-----以下毎フレーム-----


		//再生中の場所を見て、
		int playPos = gPlayer.position();
		//書き込みポインタと近づいてきたら( 書き込み位置が再生位置より後ろで、差が半バッファサイズ以下なら )
		if ( ( gWritePos > playPos ) && ( gWritePos - playPos < BUFFER_HALFSIZE ) ){
			cout << "write=" << gWritePos << "\tplay=" << playPos << "\tread=" << gReadPos << endl;
			//次を書き込み。
			int restSize = gWaveSize - gReadPos;
			int writeSize = ( restSize >= BUFFER_HALFSIZE ) ? BUFFER_HALFSIZE : restSize;
			if ( gWritePos >= BUFFER_SIZE ){ //最後まで行ったので巻き戻し
				gWritePos = 0;
			}
			bool succeeded = gPlayer.write( gWritePos, gFile.data() + 44 + gReadPos, writeSize );
			if ( !succeeded ){
				cout << "WRITING FAILED!" << endl;
			}
			gReadPos += writeSize;
			gWritePos += writeSize;
			//足りないデータは0を埋める
			if ( writeSize < BUFFER_HALFSIZE ){
				gPlayer.fillSilence( gWritePos, BUFFER_HALFSIZE - writeSize );
				gWritePos += BUFFER_HALFSIZE - writeSize;
			}
		}
		if ( isEndRequested() ){
			gFile.release();
			gPlayer.release();
		}
	}
}
