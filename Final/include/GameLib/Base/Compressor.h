#ifndef INCLUDED_GAMELIB_COMPRESSOR_H
#define INCLUDED_GAMELIB_COMPRESSOR_H

#include "GameLib/Base/Array.h"

namespace GameLib{

///圧縮展開クラス
/*
その場展開をやるには、compress()で返ってきたgetInPlaceDecompressionBufferSize
だけバッファを確保し、圧縮データを後ろに詰めて格納し、decompressを呼ぶ。
*/
class Compressor{
public:
	///圧縮
	static void compress(
		Array< char >* compressed,
		int* inPlaceDecompressionBufferSize, //その場展開時の必要バッファサイズ
		const char* data,
		int size );
	///展開。展開失敗はoutSizeが正しいかでチェックせよ。outSizeが大きすぎる場合は、メモリを破壊しており、致命傷。
	static void decompress(
		char* outData,
		int* outSize,
		const char* data,
		int size );
};

} //namespace GameLib

#endif
