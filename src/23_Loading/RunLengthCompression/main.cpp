#include <fstream>
#include <iostream>
#include <cassert> //標準のassertを使うため
using namespace std;

void compress( unsigned char* dataOut, int* sizeOut, const unsigned char* dataIn, int sizeIn );
void decompress( unsigned char* dataOut, int* sizeOut, const unsigned char* dataIn, int sizeIn );

//コマンドラインの第一引数がファイル名ね
int main( int, char** argv ){
	//とりあえず丸々読み込む。
	ifstream in( argv[ 1 ], ifstream::binary ); 
	//argv[1]が第一引数なのには慣れてもらうしかない。
	//プロジェクトのプロパティの「デバグ」のところでコマンドライン引数が設定できる。test.txtと書いてあるはずだ。
	in.seekg( 0, ifstream::end );
	int inSize = static_cast< int >( in.tellg() );
	in.seekg( 0, ifstream::beg );
	char* inData = new char[ inSize ];
	in.read( inData, inSize );

	//書き込みバッファを丸々確保。
	//この本で説明する連長圧縮は最悪でも127文字ごとに
	//「これから127文字圧縮しません」というしるしがつくだけなので、
	//ファイルサイズは128/127にまでしか増えない。
	//最悪ケースを見込んでこの容量を確保してしまう。
	//ただし、inSize * 128 / 127と馬鹿正直に計算すると、*128でintの限界を超えかねない。
	//そこで、inSize/127を足すことで処理する。また、あまりの分+1することも忘れずに。
	//なお、動作が怪しいうちは本当にこれを越えて書き込まないかチェックすること。そのためにcompress()にはoutMaxSizeも渡した方がいい。
	int outMaxSize = inSize + ( inSize / 127 ) + 1; //あまりの分+1を忘れずに。
	char* outData = new char[ outMaxSize ];

	//じゃあ圧縮するよー
	int outSize;
	compress( 
		reinterpret_cast< unsigned char* >( outData ), 
		&outSize, 
		reinterpret_cast< unsigned char* >( inData ), 
		inSize );

	//サイズここまで減りました
	cout << "FileSize: " << inSize << " -> " << outSize << endl;

	//圧縮したものを展開してちゃんと元に戻るか確かめよう。
	char* outData2 = new char[ inSize ]; //同じでいいはずだよね？
	int outSize2;
	decompress( 
		reinterpret_cast< unsigned char* >( outData2 ), 
		&outSize2, 
		reinterpret_cast< unsigned char* >( outData ), 
		outSize );

	assert( inSize == outSize2 );
	for ( int i = 0; i < inSize; ++i ){
		assert( inData[ i ] == outData2[ i ] );
	}
	cout << "succeeded." << endl;
#ifndef NDEBUG
	while ( true ){ ; }
#endif
}

//圧縮するよー
/*
連長圧縮。
圧縮領域は0x80+連長、続く文字の2バイト。
非圧縮領域は、非圧縮領域サイズ(1-127)、非圧縮文字列(1-127文字)。

書き込むのはいつか？以下の5つの場合だ
1.127文字連続したとき。0xffと連続する文字を書き込む。
2.127文字連続しなかったとき。0x7fと連続しない領域127文字を書き込む。
  ただしこの場合、最後の1文字と次の2文字、あるいは最後の2文字と次の1文字が連続している恐れがある。
  本サンプルコードではこの場合は無視して127文字書き込んでしまう。圧縮率が落ちるが、ここでさらに分岐させるとコードがとんでもなくややこしくなるからだ。
  なお1文字ないし2文字が後ろに残されているかを調べねばならなくなるからである。
3.3文字以上連続していて違う文字が来たとき。連続した長さ+0x80と、連続していた文字を書き込む。
4.連続する3文字目が来た時に、その前に非連続領域がある場合。連続していない長さと連続していない領域を書き込む。
5.ファイル末尾に来て、まだ書き込んでいない文字がある時。文字数は1ないし2のはずである。非連続として書き込む
*/
void compress( unsigned char* dataOut, int* sizeOut, const unsigned char* dataIn, int sizeIn ){
	int runBegin = 0; //つながり始めた場所
	int inPos = 0; //読み込み側の次に書き込む場所
	int outPos = 0; //書き込み側の書き込み位置
	for ( int i = 0; i <= sizeIn; ++i ){ //最後を書き込むために+1まで回す。性能を追求するならそこをループの後ろに書くべきだがコードが長くなるのでこうした。
		int runLength = i - runBegin; //連続長。つながってる数は「今-開始」。
		int rawLength = i - inPos; //非連続長。つながってない数は「今-最後に書き込んだ文字の次」。現在の文字は含まない。
		bool case5;
		bool match;
		if ( i == sizeIn ){
			match = false;
			case5 = ( rawLength > 0 ); //最後まできて書いてない文字があるなら書かねばならぬ。
		}else{
			match = ( dataIn[ i ] == dataIn[ runBegin ] );
			case5 = false;
		}
		bool case1 = ( runLength == 127 ); //ケース1
		bool case2 = ( rawLength == 127 ); //ケース2
		bool case3 = ( runLength >= 3 ) && ( !match ); //ケース3
		bool case4 = ( runLength == 2 ) && match  && ( rawLength > 2 ); //ケース4 「3文字目が来たとき」というのは「2文字続いていて、前と同じ文字が来た」と読み替えられる。「その前に非連続領域がある」は「rawLength > 2」だ。3じゃなくて2。よく考えよ。
		if ( case1 || case3 ){ //上のケース1とケース3
			dataOut[ outPos + 0 ] = static_cast< unsigned char >( 0x80 | runLength );
			dataOut[ outPos + 1 ] = dataIn[ runBegin ]; //つながる文字を書く。
			outPos += 2; //2バイト書いた
			inPos = i; //次は現在の位置から書き込みます。
			runBegin = i; //matchしようがすまいがリセット
		}else if ( case2 || case4 || case5 ){ //上のケース2,4,5
			if ( case4 ){ //4のケースの場合、書き込む文字数は連続長分削ってやる。なお、2かつ4の場合もある。125文字非連続で3文字つながった場合だ。これは4が優先である。
				rawLength -= 2; //-2,-1,0と連続している状況にあるので、書き込み文字数は2文字へらしてやる。このへんややこしいので注意。 
			}
			dataOut[ outPos ] = static_cast< unsigned char >( 0x00 | rawLength ); //つながってない長さ
			++outPos;
			for ( int j = 0; j < rawLength; ++j ){ //非圧縮領域を書き込み
				dataOut[ outPos + j ] = dataIn[ inPos + j ];
			}
			outPos += rawLength; //これだけ進む
			inPos += rawLength; //今書き込んだ文字数だけ増える。
			if ( case2 ){ //case2の場合、この直前の128文字まで書き込んでしまったのでリセットが必要。ただしこれは手抜きである。
				runBegin = i;
			}
		}
		if ( !match ){ //連続してなかったわけだから、ここから連続が開始する。
			runBegin = i;
		}
	}
	*sizeOut = outPos;
}


//展開はとっても簡単です。
//でもエラーチェックはすべき。途中で壊れたファイルを読んだ時にきちんと認識できるようにしないと実用にはならない。
void decompress( unsigned char* dataOut, int* sizeOut, const unsigned char* dataIn, int sizeIn ){
	int outPos = 0;
	for ( int i = 0; i < sizeIn; ++i ){
		if ( dataIn[ i ] & 0x80 ){ //圧縮モード
			int runLength = dataIn[ i ] - 0x80;
			for ( int j = 0; j < runLength; ++j ){
				dataOut[ outPos + j ] = dataIn[ i + 1 ];
			}
			outPos += runLength;
			i += 1; //1バイト余分に進めてやる。
		}else{ //非圧縮モード
			int rawLength = dataIn[ i ];
			for ( int j = 0; j < rawLength; ++j ){
				dataOut[ outPos + j ] = dataIn[ i + 1 + j ];
			}
			outPos += rawLength;
			i += rawLength; //ほうっておいても1は足される。生データrawLength分だけ進め、その前の1バイトは自然にまかせよう
		}
	}
	*sizeOut = outPos;
}
