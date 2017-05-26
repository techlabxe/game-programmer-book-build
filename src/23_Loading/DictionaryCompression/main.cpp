#include <fstream>
#include <iostream>
#include <cassert> //標準のassertを使うため。GameLib内のASSERTと使い方は同じ。
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
	//この本で説明する辞書圧縮は最悪でも127文字ごとに
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

	for ( int i = 0; i < inSize; ++i ){
		assert( inData[ i ] == outData2[ i ] );
	}
	assert( inSize == outSize2 );
	cout << "succeeded." << endl;
#ifndef NDEBUG
	while ( true ){ ; }
#endif
}

//よく使う最大最小
int min( int a, int b ){
	return ( a < b ) ? a : b;
}

int max( int a, int b ){
	return ( a > b ) ? a : b;
}

void compress( unsigned char* dataOut, int* sizeOut, const unsigned char* dataIn, int sizeIn ){
	int oPos = 0; //書き込み側の書き込む位置
	int i = 0;
	int unmatchBegin = 0; //非一致領域の開始位置
	while ( i < sizeIn ){
		//辞書から検索
		int matchLength = 0;
		int matchPos = 0;
		//辞書の先頭から探していく。jはiを越えない。
		//辞書の先頭。
		int dicBegin = max( i - 255, 0 ); //0より前にはなれないことに注意。だからmax()を使う
		//最大検索長
		int maxL = min( 127, sizeIn - i ); //ファイル末尾より後は検索できないので、maxLを制限する。
		for ( int j = dicBegin; j < i; ++j ){ //なお、このループの中身が計算の大半を占めている。ここを高速化することが必要になるが、結構大変だと思う。
			//一致長を調べる
			int l = 0;
			while ( l < maxL ){ //j<iで、i+l<sizeIn。よって、j+l<sizeInで、範囲内に入る。ここでj+l>=iはありうる。つまり、辞書をはみ出して検索することはありうる。しかし、それでも正しく動くのだ。図を描いて調べよう。
				//次の文字がマッチしなければ終わる
				if ( dataIn[ j + l ] != dataIn[ i + l ] ){
					break;
				}
				++l; //1文字成長
			}
			//前より長く一致したなら記録。いろんなマッチの仕方があるはずだから、最大のものを記録する。
			if ( matchLength < l ){
				matchPos = j;
				matchLength = l;
				if ( matchLength == maxL ){ //一致長が最大になったらそこで終わる。
					break;
				}
			}
		}
		//一致が3文字以上あれば圧縮モードで記録する。
		if ( matchLength >= 3 ){
			if ( unmatchBegin < i ){
				//非圧縮ヘッダ書き込み
				dataOut[ oPos ] = static_cast< unsigned char >( i - unmatchBegin );
				++oPos;
				for ( int j = unmatchBegin; j < i; ++j ){
					dataOut[ oPos ] = dataIn[ j ];
					++oPos;
				}
			}
			//圧縮部分を記録
			dataOut[ oPos + 0 ] = static_cast< unsigned char >( 0x80 + matchLength ); //長さ
			dataOut[ oPos + 1 ] = static_cast< unsigned char >( i - matchPos ); //場所
			oPos += 2;
			i += matchLength;
			unmatchBegin = i; //非一致位置は次から
		}else{ //マッチしなかった。書き込みはまとめてやるので、今は進めるだけ
			++i;
			if ( i - unmatchBegin == 127 ){ //限界数溜まってしまった。書き込む
				//非圧縮ヘッダ書き込み
				dataOut[ oPos ] = static_cast< unsigned char >( i - unmatchBegin );
				++oPos;
				for ( int j = unmatchBegin; j < i; ++j ){
					dataOut[ oPos ] = dataIn[ j ];
					++oPos;
				}
				unmatchBegin = i;
			}
		}
	}

	//非一致位置が残っていれば最後の書き込み
	if ( unmatchBegin < i ){
		//非圧縮ヘッダ書き込み
		dataOut[ oPos ] = static_cast< unsigned char >( i - unmatchBegin );
		++oPos;
		for ( int j = unmatchBegin; j < i; ++j ){
			dataOut[ oPos ] = dataIn[ j ];
			++oPos;
		}
	}
	*sizeOut = oPos; //サイズ書き込み
}

//展開はとっても簡単です。
//でもエラーチェックはすべき。途中で壊れたファイルを読んだ時にきちんと認識できるようにしないと実用にはならない。
void decompress( unsigned char* dataOut, int* sizeOut, const unsigned char* dataIn, int sizeIn ){
	int outPos = 0;
	for ( int i = 0; i < sizeIn; ++i ){
		int length;
		if ( dataIn[ i ] & 0x80 ){ //圧縮モード
			length = dataIn[ i ] - 0x80;
			int position = dataIn[ i + 1 ];
			for ( int j = 0; j < length; ++j ){
				dataOut[ outPos + j ] = dataOut[ outPos - position + j ]; //outから移すのが気持ち悪いかもしれないが、すでに書いた部分のoutは辞書なのである。
			}
			i += 1; //1バイト余分に進めてやる。
		}else{ //非圧縮モード
			length = dataIn[ i ];
			for ( int j = 0; j < length; ++j ){
				dataOut[ outPos + j ] = dataIn[ i + 1 + j ];
			}
			i += length; //ほうっておいても1は足される。生データrawLength分だけ進め、その前の1バイトは自然にまかせよう
		}
		outPos += length;
	}
	*sizeOut = outPos;
}
