//最近流行のRangeCoder。ただし、特許的に大丈夫かどうかは甚だ怪しいので注意。不安ならハフマンを使おう。

//ギリギリまでコードを単純にするために、速度は完全に度外視している。非常に遅い。
//どうすれば速くなるか考えてみるといいだろう。

#include <fstream>
#include <iostream>
#include <cassert> //標準のassertを使うため。GameLib内のASSERTと使い方は同じ。
#include "BitStream.h" //ビット単位の読み書きをするためのクラス
using namespace std;

//上の方の数も使いたいのでunsignedを多用する。
typedef unsigned U4; //4バイトのUnsignedの意味で、U4

//幅の精度。これより大きなファイルを圧縮するには工夫が必要になる。
const U4 RANGE_MAX = 0xffffffff;

void encode( unsigned char* dataOut, int* sizeOut, const unsigned char* dataIn, int sizeIn );
void decode( unsigned char* dataOut, int* sizeOut, const unsigned char* dataIn, int sizeIn );
//ビット単位で比べる関数
bool isEqual( unsigned char* a, unsigned char* b, int sizeInBit );

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
	//RangeCoderの最悪ケースは、全部8bit符号になって、そこに頻度表1024バイトと元サイズ4バイトが加わった場合。
	int outMaxSize = inSize + 256 * 4 + 4;
	char* outData = new char[ outMaxSize ];

	//じゃあ圧縮するよー
	int outSize;
	encode( 
		reinterpret_cast< unsigned char* >( outData ), 
		&outSize, 
		reinterpret_cast< unsigned char* >( inData ), 
		inSize );

	//サイズここまで減りました
	cout << "FileSize: " << inSize << " -> " << outSize << endl;

	//圧縮したものを展開してちゃんと元に戻るか確かめよう。
	char* outData2 = new char[ inSize ]; //同じでいいはずだよね？
	int outSize2;
	decode( 
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

//圧縮するよー
/*
RangeCoderによる圧縮。コメントは大量に書くし、コードをややこしくする工夫は一切入れない。
しかし、最低限算術符号の理屈をわかっていないとどうにもならないと思う。
いずれ本を書ければいいなあと思ってはいるが。
*/

void encode( unsigned char* dataOut, int* sizeOut, const unsigned char* dataIn, int sizeIn ){
	OBitStream stream( dataOut ); //出力先バッファ。実はビット単位で吐く必要はないんだが、理屈がわかりやすいようにそうしてみる。
	U4 n = sizeIn; //文字数。良く使うから一文字で別名

	//最初に元のファイルサイズを書き込む
	stream.write( n );

	//まず文字の数を数える
	U4 range[ 256 ];
	for ( U4 i = 0; i < 256; ++i ){
		range[ i ] = 0;
	}
	for ( U4 i = 0; i < n; ++i ){
		++range[ dataIn[ i ] ];
	}
	//数え終わった。これは展開の時に使うので全部書き込む。
	for ( int i = 0; i < 256; ++i ){
		stream.write( range[ i ] );
	}
	//今度は、こいつを始点と範囲に変換する。
	U4 begin[ 256 ];
	begin[ 0 ] = 0;
	for ( U4 i = 1; i < 256; ++i ){
		begin[ i ] = begin[ i - 1 ] + range[ i - 1 ];
	}
	//さあ、レンジコーダを始めるよ！
	U4 b = 0; //始点begin
	U4 r = RANGE_MAX; //幅range あまりに使用回数が多いので一文字。ごめん。
	for ( U4 i = 0; i < n; ++i ){
		U4 c = dataIn[ i ]; //文字c
		//幅/データ量を計算。unit(単位)でuとしよう。
		U4 u = r / n;
		//始点を更新
		U4 add = u * begin[ c ];
		if ( b > ( b + add ) ){ //繰り上がるぞ！このイレギュラーへの対処は相当重要。何故必要かをここで語るのは難しい。
			stream.add(); //繰り上がり。最後の桁に1を足す。
		}
		b += u * begin[ c ];
		//幅を更新
		r = u * range[ c ];
		//で、幅が0x40000000より小さくなるようなら1ビットシフトしてやる。
		while ( r <= RANGE_MAX/2 ){
			//一番上のビットを出力する。始点bが出力なのである。
			stream.write( ( b >= 0x80000000 ) ? true : false );
			//幅と始点を両方倍。
			r <<= 1;
			b <<= 1;
		}
	}
	//残った32bitを出力
	for ( int i = 0; i < 32; ++i ){
		stream.write( ( b >= 0x80000000 ) ? true : false );
		b <<= 1;
	}

	*sizeOut = stream.sizeInByte() + 4; //バイトでサイズをもらって、先頭の文字数の4を足す。
}

//展開の方が重い。
//範囲の検索が入るからだ。でも二分検索を入れることで10倍くらいはすぐ速くなる。
void decode( unsigned char* dataOut, int* sizeOut, const unsigned char* dataIn, int sizeIn ){
	IBitStream stream( dataIn );
	//まず32bit取り出す。これがファイルサイズです。
	U4 n = stream.readU4();

	//各文字の頻度を取り出す
	U4 range[ 256 ];
	for ( U4 i = 0; i < 256; ++i ){
		range[ i ] = stream.readU4();
	}
	//今度は、こいつを始点と範囲に変換する。
	U4 begin[ 256 ];
	begin[ 0 ] = 0;
	for ( U4 i = 1; i < 256; ++i ){
		begin[ i ] = begin[ i - 1 ] + range[ i - 1 ];
	}
	//さあ、展開を始めるよ！
	U4 b = 0;
	//始点は最初の32ビットを読み出す
	for ( int i = 0; i < 32; ++i ){
		b <<= 1;
		b |= ( stream.read() ) ? 1 : 0;
	}
	U4 r = RANGE_MAX; //幅range あまりに使用回数が多いので一文字。ごめん。
	U4 pos = 0; //書き込み位置
	for ( U4 i = 0; i < n; ++i ){
		//幅/データ量を計算。unit(単位)でuとしよう。
		U4 u = r / n;
		//まずやるのは検索。現在のbがどこに相当するのかを調べる。
		int c = -1; //文字
		for ( U4 j = 0; j < 256; ++j ){
			if ( b < ( ( begin[ j ] + range[ j ] ) * u ) ){ //始点+幅より小さければその領域にいる。
				c = j; //発見
				break;
			}
		}
		assert( c != -1 ); //何か見つかっているはずだぞ？
		//見つかった文字はcだ！
		dataOut[ pos ] = static_cast< unsigned char >( c );
//		cout << static_cast< char >( c ); //出てきた文字を書いてみよう。
		++pos;
		//始点からこの場所を引く。
		b -= u * begin[ c ];
		//幅も更新
		r = u * range[ c ];
		//で、幅が0x40000000より小さくなるようなら1ビットシフトしてやる。
		while ( r <= RANGE_MAX/2 ){
			//幅と始点を両方倍。
			r <<= 1;
			b <<= 1;
			//bの一番下のビットに入力を取ってきて入れる。
			b |= ( stream.read() ) ? 1 : 0;
		}
	}
//	cout << endl;
	assert( pos == n ); //ちゃんとぴったりデコードできたか？
	assert( stream.positionInByte() <= sizeIn ); //読みすぎてない？
	*sizeOut = pos;
}
