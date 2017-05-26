#include <fstream>
#include <iostream>
#include <cassert>
#include "GameLib/Base/Compressor.h"
using namespace std;

class DictionaryCompressor{
public:
	static bool compress( 
		int* outSize, 
		int* necessaryBufferSize,
		bool* compressed,
		ofstream* oStream, 
		ifstream* iStream,
		double threshold );
private:
	static bool compress( 
		unsigned char* outData, 
		int* outSize, 
		int* necessaryBufferSize,
		int outBufferSize,
		const unsigned char* inData, 
		int inSize );
	static bool decompress( 
		unsigned char* outData, 
		int* outSize,
		int outBufferSize,
		const unsigned char* inData, 
		int inSize );
};

bool DictionaryCompressor::compress(
int* writtenSize, 
int* necessaryBufferSize,
bool* compressed,
ofstream* oStream, 
ifstream* iStream,
double threshold ){
	//本当は分割読みしないとまずいが面倒なので一括
	iStream->seekg( 0, ifstream::end );
	int inSize = static_cast< int >( iStream->tellg() );
	iStream->seekg( 0, ifstream::beg );
	char* inData = new char[ inSize ];
	iStream->read( inData, inSize );

	//書き込み先用意
	int outBufferSize = inSize + ( inSize / 127 ) + 1; //あまりの分+1を忘れずに。
	char* outData = new char[ outBufferSize ];
	if ( !outData ){
		cerr << "memory allocation failed : " << outBufferSize << endl;
		return false;
	}
	//じゃあ圧縮するよー
	int outSize;
	bool succeeded = compress( 
		reinterpret_cast< unsigned char* >( outData ), 
		&outSize, 
		necessaryBufferSize,
		outBufferSize,
		reinterpret_cast< unsigned char* >( inData ), 
		inSize );
	assert( succeeded );
#ifndef NDEBUG
	{
		//圧縮したものを展開してちゃんと元に戻るか確かめよう。
		char* outData2 = new char[ inSize ]; //同じでいいはずだよね？
		int outSize2;
		succeeded = decompress( 
			reinterpret_cast< unsigned char* >( outData2 ), 
			&outSize2, 
			inSize,
			reinterpret_cast< unsigned char* >( outData ), 
			outSize );
		assert( succeeded );
		for ( int i = 0; i < inSize; ++i ){
			assert( inData[ i ] == outData2[ i ] );
		}
		assert( inSize == outSize2 );
		delete[] outData2;
		outData2 = 0;
	}
#endif
	//書きこみ
	double t = inSize;
	t *= threshold;
	t /= 100.0;
	if ( outSize < t ){ //圧縮バージョンを書き込み
		oStream->write( outData, outSize );
		*writtenSize = outSize;
		*compressed = true;
	}else{
		oStream->write( inData, inSize );
		*writtenSize = inSize;
		*necessaryBufferSize = inSize;
		*compressed = false;
	}
	delete[] inData;
	delete[] outData;
	inData = 0;
	outData = 0;
	return true;
}

//ビット割り当て
const int DIC_BITS = 10; //位置bit数
//以下自動計算定数
const int LENGTH_BITS = 16 - 1 - DIC_BITS; //長さ
const int DIC_MASK = ( 1 << DIC_BITS ) - 1;
const int DIC_MASK_HIGH = DIC_MASK & 0xffffff00; //下8bitをつぶしたもの
const int DIC_MASK_SHIFTED = ( DIC_MASK >> 8 ) << LENGTH_BITS; 
const int LENGTH_MASK = ( 1 << LENGTH_BITS ) - 1;
const int DIC_SIZE = DIC_MASK + 3; //辞書サイズ
const int MAX_LENGTH = LENGTH_MASK + 3; //最大一致長

//よく使う最小値と最大値
inline int min( int a, int b ){
	return ( a < b ) ? a : b;
}

inline int max( int a, int b ){
	return ( a > b ) ? a : b;
}

//圧縮するよー
/*
辞書圧縮。LZ77の何か。
圧縮領域は2バイトで、位置と長さに分配してある。
1バイト目 : 0x80+サイズ(5bit)+位置の上位ビット(2bit)、
2バイト目 : 位置の下位8bit。
非圧縮領域は、非圧縮領域サイズ、非圧縮文字列(1から128)文字

圧縮領域は、位置、長さ共に-3した数を格納してある。
展開時は取り出してから3を足す。
非圧縮領域は長さを-1してあり、取り出す時には+1する。

23章のサンプルに比べて、エラー処理や、圧縮率向上の工夫でコードが長くなっている。
*/
bool DictionaryCompressor::compress( 
unsigned char* oData, 
int* oSize, 
int* oNecessaryBufferSize,
int oBufferSize, 
const unsigned char* iData, 
int iSize ){
	int oPos = 0; //書き込み側の書き込む位置
	int maxPosDiff = 0; //展開時に、書き込みポインタが読み込みポインタを追い抜く量の最大値。その場展開するためには、この分だけ大きな展開バッファが必要。
	int i = 0;
	int unmatchBegin = 0; //非一致領域の開始位置
	while ( i < iSize ){
		//辞書から検索
		int matchLength = 0;
		int matchPos = 0;
		//辞書の先頭。
		int dicBegin = max( i - DIC_SIZE, 0 ); //0より前にはなれないことに注意。だからmax()を使う
		//最大検索長
		int maxL = min( MAX_LENGTH, iSize - dicBegin ); //入力の最後を越えて検索はできない。
		//辞書の先頭から探していく。ここで下手な工夫をすると特許に引っかかるのでやっていない。
		int jEnd = i - 3; //検索先頭は3文字以上前から始める。3文字以上マッチしない限り圧縮しないからだ。
		//ここでは元のサンプルと違ってループを一重にしている。不一致でjをl-1だけ戻す作りだ。こちらの方がわかりにくいが、最後までマッチした時にすぐ抜けてくれるので特定の種類のファイル(真っ白な画像とか)で速くなる。
		int j = dicBegin;
		int l = 0;
		while ( ( j < jEnd ) && ( l < maxL ) ){
			if ( iData[ j ] == iData[ i + l ] ){
				++j;
				++l;
			}else{
				//前より長く一致したなら記録。いろんなマッチの仕方があるはずだから、最大のものを記録する。
				if ( matchLength < l ){
					matchPos = j - l;
					matchLength = l;
				}
				j -= l - 1; //lが0でも1文字は進む。lが増えるとたくさん戻る。
				l = 0;
			}
		}
		if ( matchLength < l ){ //最後まで来てしまった場合に対処
			matchPos = j - l;
			matchLength = l;
		}
		//さて、一致が3文字以上あれば圧縮モードで記録する。
		if ( matchLength >= 3 ){
			if ( unmatchBegin < i ){
				if ( ( oPos + i - unmatchBegin + 1 ) > oBufferSize ){ //書き込みバッファ不足エラー検出 最後の+1はヘッダ分
					return false; //失敗
				}
				//非圧縮ヘッダ書き込み
				oData[ oPos ] = static_cast< unsigned char >( i - unmatchBegin - 1 ); //最低1なので1引いて保存
				++oPos;
				for ( int j = unmatchBegin; j < i; ++j ){
					oData[ oPos ] = iData[ j ];
					++oPos;
				}
			}
			if ( ( oPos + 2 ) > oBufferSize ){ //書き込みバッファ不足エラー検出
				return false; //失敗
			}
			//圧縮部分を記録
			int wl = matchLength - 3; //3引いて格納
			int wp = i - matchPos - 3; //3引いて格納
			int tmp = 0x80 | wl; //長さに圧縮フラグを追加
			tmp |= ( wp & DIC_MASK_HIGH ) >> ( 8 - LENGTH_BITS ); //maskと&し、これをサイズに使っているビットの分だけずらす。
			oData[ oPos + 0 ] = static_cast< unsigned char >( tmp );
			oData[ oPos + 1 ] = static_cast< unsigned char >( wp & 0xff );
			oPos += 2;
			i += matchLength;
			unmatchBegin = i; //非一致位置は次から
		}else{ //マッチしなかった。書き込みはまとめてやるので、今は進める。
			++i;
			if ( i - unmatchBegin == 128 ){ //限界数溜まってしまった。書き込む
				if ( ( oPos + i - unmatchBegin + 1 ) > oBufferSize ){ //書き込みバッファ不足エラー検出 最後の+1はヘッダ分
					return false; //失敗
				}
				//非圧縮ヘッダ書き込み
				oData[ oPos ] = static_cast< unsigned char >( i - unmatchBegin - 1 ); //最低1なので1引いて保存
				++oPos;
				for ( int j = unmatchBegin; j < i; ++j ){
					oData[ oPos ] = iData[ j ];
					++oPos;
				}
				unmatchBegin = i;
			}
		}
		//書き込み位置が追い抜いた量を更新
		maxPosDiff = max( maxPosDiff, i - oPos );
	}

	//非一致位置が残っていれば最後の書き込み
	if ( unmatchBegin < i ){
		if ( ( oPos + i - unmatchBegin + 1 ) > oBufferSize ){ //書き込みバッファ不足エラー検出 最後の+1はヘッダ分
			return false; //失敗
		}
		//非圧縮ヘッダ書き込み
		oData[ oPos ] = static_cast< unsigned char >( i - unmatchBegin - 1 ); //最低1なので1引いて保存
		++oPos;
		for ( int j = unmatchBegin; j < i; ++j ){
			oData[ oPos ] = iData[ j ];
			++oPos;
		}
	}

	*oNecessaryBufferSize = max( maxPosDiff + oPos, iSize );
	*oSize = oPos;
	return true;
}

//展開はとっても簡単です。でもエラーチェックでちょっと長くなっている。
bool DictionaryCompressor::decompress( 
unsigned char* oData, 
int* oSize,
int oBufferSize,
const unsigned char* iData, 
int iSize ){
	int oPos = 0;
	for ( int i = 0; i < iSize; ++i ){
		int length;
		if ( iData[ i ] & 0x80 ){ //圧縮モード
			length = iData[ i ] & LENGTH_MASK;
			length += 3; //3文字少なく記録されている。
			int position = ( ( iData[ i ] & DIC_MASK_SHIFTED ) << ( 8 - LENGTH_BITS ) ) | iData[ i + 1 ]; //複雑なのでよく分解して考えよう
			position += 3; //3文字少なく記録されている。
			if ( oPos + length > oBufferSize ){ //書き込みバッファサイズチェック
				return false;
			}
			for ( int j = 0; j < length; ++j ){
				oData[ oPos + j ] = oData[ oPos - position + j ]; //outから移すのが気持ち悪いかもしれないが、すでに書いた部分のoutは辞書なのである。
			}
			i += 1; //1バイト余分に進めてやる。
		}else{ //非圧縮モード
			length = iData[ i ] + 1; //1文字少なく記録してある
			if ( oPos + length > oBufferSize ){ //書き込みバッファサイズチェック
				return false;
			}
			for ( int j = 0; j < length; ++j ){
				oData[ oPos + j ] = iData[ i + 1 + j ];
			}
			i += length; //ほうっておいても1は足される。生データlength分だけ進め、その前の1バイトは自然にまかせよう
		}
		oPos += length;
	}
	*oSize = oPos;
	return true;
}
