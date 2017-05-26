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
			if ( src & 0x80000000 ){ //アルファチャネルが128以上なら
				unsigned* dst = &vram[ ( y + dstY ) * windowWidth + ( x + dstX ) ];
				*dst = src;
			}
		}
	}
}
