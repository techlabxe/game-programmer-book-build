#include "Image.h"
#include "File.h"
#include "Vector2.h"

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
	SAFE_DELETE( mData );
}

int Image::width() const {
	return mWidth;
}

int Image::height() const {
	return mHeight;
}

unsigned Image::pixel( const Vector2& v ) const {
	double x = v.x;
	double y = v.y;
	x += ( x > 0 ) ? 0.5 : -0.5;
	y += ( y > 0 ) ? 0.5 : -0.5;
	int xi = static_cast< int >( x );
	int yi = static_cast< int >( y );
	if ( xi < 0 || xi >= mWidth ){
		return 0;
	}else if ( yi < 0 || yi > mHeight ){
		return 0;
	}else{
		return mData[ yi * mWidth + xi ];
	}
}
