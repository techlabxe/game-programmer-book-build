#include "Image.h"
#include "File.h"

#include "GameLib/Framework.h"
using namespace GameLib;

Image::Image( const char* filename ) : 
mWidth( 0 ),
mHeight( 0 ),
mData( 0 ){
	File f( filename );
	mHeight = f.getUnsigned( 12 );
	mWidth = f.getUnsigned( 16 );
	mData = new unsigned[ mWidth * mHeight ];
	for ( int i = 0; i < mWidth * mHeight; ++i ){
		mData[ i ] = f.getUnsigned( 128 + i * 4 );
	}
}

Image::~Image(){
	delete[] mData;
	mData = 0;
}

int Image::width() const {
	return mWidth;
}

int Image::height() const {
	return mHeight;
}

//#define USE_FLOAT_VERSION
#define USE_INTEGER_VERSION
//#define USE_OPTIMIZED_VERSION_0
//#define USE_OPTIMIZED_VERSION_1

#ifdef USE_FLOAT_VERSION
//アルファブレンド付き(doubleバージョン。遅いが確実に正確)
//高速化する時はこれと結果を比べるのを忘れないように。
void Image::draw(
int dstX, 
int dstY, 
int srcX, 
int srcY, 
int width, 
int height ) const {
	unsigned* vram = Framework::instance().videoMemory();
	unsigned windowWidth = Framework::instance().width();
	for ( int y = 0; y < height; ++y ){
		for ( int x = 0; x < width; ++x ){
			unsigned src = mData[ ( y + srcY ) * mWidth + ( x + srcX ) ];
			unsigned* dst = &vram[ ( y + dstY ) * windowWidth + ( x + dstX ) ];
			double srcA = static_cast< double >( ( src & 0xff000000 ) >> 24 ) / 255.f;
			double srcR = static_cast< double >( ( src & 0x00ff0000 ) >> 16 );
			double srcG = static_cast< double >( ( src & 0x0000ff00 ) >> 8 );
			double srcB = static_cast< double >( ( src & 0x000000ff ) >> 0 );

			double dstR = static_cast< double >( ( *dst & 0x00ff0000 ) >> 16 );
			double dstG = static_cast< double >( ( *dst & 0x0000ff00 ) >> 8 );
			double dstB = static_cast< double >( ( *dst & 0x000000ff ) >> 0 );

			double r = ( srcR - dstR ) * srcA + dstR;
			double g = ( srcG - dstG ) * srcA + dstG;
			double b = ( srcB - dstB ) * srcA + dstB;
			*dst = static_cast< unsigned >( r ) << 16;
			*dst |= static_cast< unsigned >( g ) << 8;
			*dst |= static_cast< unsigned >( b );
		}
	}
}
#endif

#ifdef USE_INTEGER_VERSION
//アルファブレンド付き
void Image::draw(
int dstX, 
int dstY, 
int srcX, 
int srcY, 
int width, 
int height ) const {
	unsigned* vram = Framework::instance().videoMemory();
	unsigned windowWidth = Framework::instance().width();
	for ( int y = 0; y < height; ++y ){
		for ( int x = 0; x < width; ++x ){
			unsigned src = mData[ ( y + srcY ) * mWidth + ( x + srcX ) ];
			unsigned* dst = &vram[ ( y + dstY ) * windowWidth + ( x + dstX ) ];
			unsigned srcA = ( src & 0xff000000 ) >> 24;
			unsigned srcR = src & 0xff0000;
			unsigned srcG = src & 0x00ff00;
			unsigned srcB = src & 0x0000ff;
			unsigned dstR = *dst & 0xff0000;
			unsigned dstG = *dst & 0x00ff00;
			unsigned dstB = *dst & 0x0000ff;
			unsigned r = ( srcR - dstR ) * srcA / 255 + dstR;
			unsigned g = ( srcG - dstG ) * srcA / 255 + dstG;
			unsigned b = ( srcB - dstB ) * srcA / 255 + dstB;
			*dst = ( r & 0xff0000 ) | ( g & 0x00ff00 ) | b;
		}
	}
}
#endif

#ifdef USE_OPTIMIZED_VERSION_0
//アルファブレンド付き(並列化版)
/*
青と赤を一気にやる。
間に8bit分の隙間があるので加減算と乗算の範囲では混ざらない。
ただし除算が入ると上の桁の除算の余りが下の桁に混入して
結果が狂う。そこで、除算のみは別々に行う。
(A - B) * a/255 + B
を、
((A - B) * a + 255*B)
と変形し、桁を分離してから独立に除算する。

10進法の例を出してみよう。
(306-703)*7/9 + 703 = 394 -> 304
で、
(3-7)*7/9+7 = 3
(6-3)*7/9+3 = 5
で異なってしまうが、前者を
((306-703)*7 + 9*703) = 3548
で止め、上下2桁づつに分割すると、
3500と48になり、独立に9で除すると、
388と5。
上の桁の下位を切り捨ててから合成すると305となり、一致する。

ついでなので、添え字演算も高速化しておいた。
*/
void Image::draw(
int dstX, 
int dstY, 
int srcX, 
int srcY, 
int width, 
int height ) const {
	unsigned* vram = Framework::instance().videoMemory();
	unsigned windowWidth = Framework::instance().width();
	int dx = srcY * mWidth + srcX;
	int dy = dstY * windowWidth + dstX;
	for ( int y = 0; y < height; ++y ){
		for ( int x = 0; x < width; ++x ){
			unsigned src = mData[ sy + x ];
			unsigned* dst = &vram[ dy + x ];
			unsigned srcA = ( src & 0xff000000 ) >> 24;
			unsigned srcRB = src & 0xff00ff;
			unsigned srcG = src & 0x00ff00;
			unsigned dstRB = *dst & 0xff00ff;
			unsigned dstG = *dst & 0x00ff00;
			unsigned rb = ( srcRB - dstRB ) * srcA  + dstRB * 255;
			unsigned r = ( rb & 0xffff0000 ) / 255;
			unsigned b = ( rb & 0x0000ffff ) / 255;
			unsigned g = ( srcG - dstG ) * srcA / 255 + dstG;
			*dst = ( r & 0xff0000 ) | ( g & 0x00ff00 ) | b;
		}
		sy += mWidth;
		dy += windowWidth;
	}
}
#endif

#ifdef USE_OPTIMIZED_VERSION_1
//アルファブレンド付き(並列化版)
/*
VERSION_0に加えて、除算をシフトに変換する。
アルファが0-255だと255で割らねばならないが、
もし256で割れれば除算をシフトに置き換えられる。
この時、255->255,0->0は守りたい。
変なものが混ざればインチキがばれてしまうからだ。
人間は暗い方の色変化には鈍感なので、
1以上は+1してしまおう。
そして、256で割る場合は除算でもゴミが入らないため、
上のように除算を赤と青別々でやる必要もない。
*/
void Image::draw(
int dstX, 
int dstY, 
int srcX, 
int srcY, 
int width, 
int height ) const {
	unsigned* vram = Framework::instance().videoMemory();
	unsigned windowWidth = Framework::instance().width();
	int sy = srcY * mWidth + srcX;
	int dy = dstY * windowWidth + dstX;
	for ( int y = 0; y < height; ++y ){
		for ( int x = 0; x < width; ++x ){
			unsigned src = mData[ sy + x ];
			unsigned* dst = &vram[ dy + x ];
			unsigned srcA = ( src & 0xff000000 ) >> 24;
			srcA += ( srcA >= 1 ) ? 1 : 0; //1以上なら1を足す。
			unsigned srcRB = src & 0xff00ff;
			unsigned srcG = src & 0x00ff00;
			unsigned dstRB = *dst & 0xff00ff;
			unsigned dstG = *dst & 0x00ff00;
			unsigned rb = ( ( ( srcRB - dstRB ) * srcA ) >> 8 ) + dstRB;
			unsigned g = ( ( ( srcG - dstG ) * srcA ) >> 8 ) + dstG;
			*dst = ( rb & 0xff00ff ) | ( g & 0x00ff00 );
		}
		sy += mWidth;
		dy += windowWidth;
	}
}
#endif