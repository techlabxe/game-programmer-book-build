#include "GameLib/GameLib.h"
#include "GameLib/Base/Compressor.h"
#include <fstream>
using namespace std;
using namespace GameLib;

//圧縮
bool compress(
int* writtenSize, 
int* inplaceDecompressionBufferSize,
bool* compressed,
ofstream* oStream, 
ifstream* iStream,
double threshold ){
	//本当は分割読みしないとまずいが面倒なので一括
	iStream->seekg( 0, ifstream::end );
	int inSize = static_cast< int >( iStream->tellg() );
	iStream->seekg( 0, ifstream::beg );
	Array< char > inData( inSize + 1 ); //0バイトでも動くようにインチキ
	iStream->read( &inData[ 0 ], inSize );

	//じゃあ圧縮するよー
	Array< char > outData;
	Compressor::compress( 
		&outData,
		inplaceDecompressionBufferSize,
		&inData[ 0 ],
		inSize );
	int outSize = outData.size();
//#ifndef NDEBUG //圧縮したものを展開してちゃんと元に戻るか確かめよう。
	if ( outSize > 0 ){
		{
			Array< char > decompressed( inSize );
			int decompressedSize;
			Compressor::decompress( 
				&decompressed[ 0 ],
				&decompressedSize,
				&outData[ 0 ],
				outSize );
			STRONG_ASSERT( inSize == decompressedSize );
			for ( int i = 0; i < inSize; ++i ){
				STRONG_ASSERT( inData[ i ] == decompressed[ i ] );
			}
		}
		{
			//その場展開のテスト。後ろに詰める。
			Array< char > decompressed( *inplaceDecompressionBufferSize );
			int offset = *inplaceDecompressionBufferSize - outSize;
			for ( int i = 0; i < outSize; ++i ){
				decompressed[ offset + i ] = outData[ i ];
			}
			int decompressedSize;
			Compressor::decompress( 
				&decompressed[ 0 ],
				&decompressedSize,
				&decompressed[ 0 ] + offset,
				outSize );
			STRONG_ASSERT( inSize == decompressedSize );
			for ( int i = 0; i < inSize; ++i ){
				STRONG_ASSERT( inData[ i ] == decompressed[ i ] );
			}
		}
	}
//#endif
	//書きこみ
	double t = inSize;
	t *= threshold;
	t /= 100.0;
	if ( outSize < t ){ //圧縮バージョンを書き込み
		oStream->write( &outData[ 0 ], outSize );
		*writtenSize = outSize;
		*compressed = true;
	}else{
		oStream->write( &inData[ 0 ], inSize );
		*writtenSize = inSize;
		*inplaceDecompressionBufferSize = inSize;
		*compressed = false;
	}
	return true;
}

