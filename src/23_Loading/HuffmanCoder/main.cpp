//ギリギリまでコードを単純にするために、速度は完全に度外視している。非常に遅い。
//どうすれば速くなるか考えてみるといいだろう。

#include <fstream>
#include <iostream>
#include <cassert> //標準のassertを使うため。GameLib内のASSERTと使い方は同じ。
#include "BitStream.h" //ビット単位の読み書きをするためのクラス
using namespace std;

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
	//ハフマン符号の最悪ケースは、全部8bit符号になって、そこに符号表が加わった場合。
	//符号表は符号長1byte+符号最大255bit(8byte)で、9*256バイト。それに元サイズ4バイト
	int outMaxSize = inSize + 9 * 256 + 4;
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
ハフマン圧縮。最初にやることはそれぞれの文字の回数を数えること。
その後は木構成をがんばる。
*/

//ハフマン木のノード。二分木なので左右のポインタがあり、カウントを持ち、文字も持つ。文字がない時は-1。
class Node{
public:
	Node() : mLeft( 0 ), mRight( 0 ), mCount( 0xffffffff ), mLetter( -1 ){} //カウントは無限にしとく。

	//符号を作っていきます。
	void makeCodeTable( int* lengths, unsigned char* codes, unsigned char* currentCode, int currentCodeLength ){
		//unsigned char一個に8bit入るので、8で割ってバイトを確定、余りでシフトを確定。
		int index = currentCodeLength / 8;
		int shift = currentCodeLength % 8;
		//子がいれば、子に丸投げ
		if ( mLeft ){
			//左なので0を現在の符号に追加
			currentCode[ index ] &= ~( 1 << shift ); //ビットを0に。
			mLeft->makeCodeTable( lengths, codes, currentCode, currentCodeLength + 1);
		}
		if ( mRight ){
			//右なので1を現在の符号に追加
			currentCode[ index ] |= ( 1 << shift ); //ビットを1に。
			mRight->makeCodeTable( lengths, codes, currentCode, currentCodeLength + 1 );
		}
		//もしこいつが文字なら文字を出力せねばならぬ。
		if ( mLetter != -1 ){
			lengths[ mLetter ] = currentCodeLength;
			for ( int i = 0; i < 8; ++i ){
				codes[ mLetter * 8 + i ] = currentCode[ i ];
			}
		}
	}


	Node* mLeft;
	Node* mRight;
	unsigned mCount; //この節のカウント(0xffffffffがもてるようにunsigned)
	int mLetter; //文字(-1を持つためにint。ケチるならshortにしよう)
};


void encode( unsigned char* dataOut, int* sizeOut, const unsigned char* dataIn, int sizeIn ){
	//数える
	int counts[ 256 ];
	for ( int i = 0; i < 256; ++i ){
		counts[ i ] = 0;
	}
	for ( int i = 0; i < sizeIn; ++i ){
		++counts[ dataIn[ i ] ];
	}
	//数え終わった。木を作ります。
	//木はまず256個の節を並べることから始める。
	//節配列を用意するが、節の数は最大いくつになるだろう？

	//帰納法的に考える。
	//文字が1個の時には節は1個。
	//ここに文字を一個増やす時に何が起こるかと言えば、最大で二個節が増える
	//左右に枝を出して格納するから。これを255回繰り返すんだから、511個の節が最大。でも512個用意しておく。
	Node nodes[ 512 ];
	//まず最初の256個のノードを初期化します。
	for ( int i = 0; i < 256; ++i ){
		nodes[ i ].mCount = counts[ i ];
		nodes[ i ].mLetter = i;
	}
	int nodeNumber = 256; //現在256個使ってます。

	//さあ、木を構築しよう！
	//255回回せば全部の節がくっつきます。これは証明できるのでしてみよう。
	//要するに毎回節を一個使うわけで、511個になった時に使い切るのだから、そこで終わるのだ。
	//そして、木の形がどうであれ、255個の端ノードを持つ木は絶対に511個の節を持つのである。
	for ( int j = 0; j < 255; ++j ){
		//まずやること。カウントが一番小さい奴と、二番目に小さい奴を探す。
		//関係なくなった節はカウントが0xffffffffになっているのでスキップされる。
		//なお、511番目はダミー。すごくでかいカウントが入れっぱなしになっているので、これと
		int merged0 = 511; //一番カウントが小さい節
		int merged1 = 511; //二番目にカウントが小さい節
		for ( int i = 0; i < nodeNumber; ++i ){ //この検索は賢い方法を使うとあっさり高速化できるが、コードが長くなるのでやらない。
			if ( nodes[ i ].mCount < nodes[ merged1 ].mCount ){
				if ( nodes[ i ].mCount < nodes[ merged0 ].mCount ){
					merged1 = merged0; //2位を今までの1位に
					merged0 = i; //1位を交換
				}else{
					merged1 = i; //2位だけ交換
				}
			}
		}
		//1位と2位をくっつけた節を作る。
		Node* newNode = &nodes[ nodeNumber ];
		++nodeNumber;
		newNode->mLeft = &nodes[ merged0 ];
		newNode->mRight = &nodes[ merged1 ];
		newNode->mCount = nodes[ merged0 ].mCount + nodes[ merged1 ].mCount; //カウントは和になる
		nodes[ merged0 ].mCount = nodes[ merged1 ].mCount = 0xffffffff; //くっつけられた奴のカウントはいらないので無限に戻す。
	}
	//木ができた！
	//このまま圧縮を開始してもいいのだが、
	//できた符号を配列に入れておいた方が後の処理が楽なので、そうしておく。
	int lengths[ 256 ]; //符号長
	unsigned char codes[ 256 * 8 ]; //符号(最大8バイトですから)
	//符号を初期化
	for ( int i = 0; i < 256; ++i ){
		for ( int j = 0; j < 8; ++j ){
			codes[ i * 8 + j ] = 0;
		}
	}
	//根っこは最後に作った節で、これは510番と決まっている。510番から再帰で符号表を作ろう。
	int currentCodeLength = 0;
	unsigned char currentCode[ 8 ];
	for ( int i = 0; i < 8; ++i ){
		currentCode[ i ] = 0;
	}
	nodes[ 510 ].makeCodeTable( lengths, codes, currentCode, currentCodeLength ); //最後の引数は今の深さ

	//できた符号表を表示してみよう
	for ( int i = 0; i < 256; ++i ){
		cout << i << "\t" << lengths[ i ] << "\t";
		for ( int j = 0; j < lengths[ i ]; ++j ){
			cout << ( ( codes[ i * 8 + ( j / 8 ) ] & ( 1 << ( j % 8 ) ) ) >> ( j % 8 ) );
		}
		cout << endl;
	}

	//符号表ができた！書き込み開始。
	OBitStream stream( dataOut ); //出力先バッファを当てて
	
	//最初に元のファイルサイズを書き込む
	stream.write( static_cast< unsigned >( sizeIn ) );

	//符号表を書き込もう。
	for ( int i = 0; i < 256; ++i ){
		unsigned char l = static_cast< unsigned char >( lengths[ i ] );
		stream.write( &l, 8 ); //符号長は8bit固定
		stream.write( &codes[ i * 8 ], lengths[ i ] ); //符号は可変長
	}
	
	//圧縮開始！とっても単純に書ける。置換して書き込むだけだから。
	for ( int i = 0; i < sizeIn; ++i ){
		int c = dataIn[ i ];
		stream.write( &codes[ c * 8 ], lengths[ c ] );
	}
	*sizeOut = stream.sizeInByte() + 4; //バイトでサイズをもらって、先頭の文字数の4を足す。
}

//実は馬鹿正直に書くと展開の方が重い。
//1ビット読む度に符号表に探しているものがないか調べる羽目になるからだ。
void decode( unsigned char* dataOut, int* sizeOut, const unsigned char* dataIn, int sizeIn ){
	IBitStream stream( dataIn );
	//まず32bit取り出す。これがファイルサイズです。
	int origSize = stream.readS4();

	//符号表を作ります。
	int lengths[ 256 ]; //符号長
	unsigned char codes[ 256 * 8 ]; //符号(最大8バイトですから)
	for ( int i = 0; i < 256 * 8; ++i ){
		codes[ i ] = 0; //コード0初期化
	}
	for ( int i = 0; i < 256; ++i ){
		unsigned char buf[ 8 ];
		stream.read( buf, 8 ); //符号長
		lengths[ i ] = buf[ 0 ];
		stream.read( &codes[ i * 8 ], lengths[ i ] ); //符号読み込み
	}
	//読んだ符号表を表示してみよう
	for ( int i = 0; i < 256; ++i ){
		cout << i << "\t" << lengths[ i ] << "\t";
		for ( int j = 0; j < lengths[ i ]; ++j ){
			cout << ( ( codes[ i * 8 + ( j / 8 ) ] & ( 1 << ( j % 8 ) ) ) >> ( j % 8 ) );
		}
		cout << endl;
	}

	//さあ、中身を読み込もう。
	int pos = 0; //書き込み位置
	for ( int i = 0; i < origSize; ++i ){
		//0初期化した領域を作って、
		unsigned char code[ 8 ];
		for ( int j = 0; j < 8; ++j ){
			code[ j ] = 0;
		}
		//1ビット読んでは符号表と比べてマッチしているか調べる
		bool found = false;
		for ( int j = 0; j < 255; ++j ){ //最大255ビットです。
			int index = j / 8;
			int shift = j % 8;
			if ( stream.read() ){ //1bit読んで1なら1を書き込む
				code[ index ] |= ( 1 << shift );
			}
			//符号表から探します。
			int length = j + 1;
			for ( int k = 0; k < 256; ++k ){
				if ( lengths[ k ] == length ){ //長さが同じで、
					if ( isEqual( &codes[ k * 8 ], code, length ) ){ //一緒なら
						dataOut[ pos ] = static_cast< unsigned char >( k ); //文字を出力
//						cout << static_cast< char >( k ); //デコードした文字を出してみよう
						++pos;
						found = true;
						break;
					}
				}
			}
			if ( found ){
				break;
			}
		}
		assert( found ); //見つからないことはありえない
	}
	cout << endl;
	assert( pos == origSize ); //ちゃんとぴったりデコードできたか？
	assert( stream.positionInByte() <= sizeIn );
	*sizeOut = pos;
}


//ビット比較関数
bool isEqual( unsigned char* a, unsigned char* b, int sizeInBit ){
	//まずバイト単位でできる分だけやる
	int sizeInByte = sizeInBit / 8;
	for ( int i = 0; i < sizeInByte; ++i ){
		if ( a[ i ] != b[ i ] ){
			return false;
		}
	}
	sizeInBit = sizeInBit % 8; //余りビット数
	if ( sizeInBit > 0 ){ //余りがあれば、余りを比べる
		//問題のビット以外を0にするための1の並びを作る。
		unsigned char mask = static_cast< unsigned char >( 0xff >> ( 8 - sizeInBit ) ); //例えば6bitあまってるなら、00111111が作りたいわけだ。11111111をシフトして作る。
		return ( ( a[ sizeInByte ] & mask ) == ( b[ sizeInByte ] & mask ) );
	}else{ //余りがないので、ここまで来たということはtrueだということだ。
		return true;
	}
}
