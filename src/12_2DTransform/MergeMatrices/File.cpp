#include "GameLib/GameLib.h"
#include "File.h"
#include <fstream>
using namespace std;

File::File( const char* filename ) : mSize( 0 ), mData( 0 ){
	ifstream in( filename, ifstream::binary );
	if ( in ){
		in.seekg( 0, ifstream::end );
		mSize = static_cast< int >( in.tellg() );
		in.seekg( 0, ifstream::beg );
		mData = new char[ mSize ];
		in.read( mData, mSize );
	}else{
		HALT( "can't open file." ); //’v–½‚¾‚©‚ç~‚ß‚éB
	}
}

File::~File(){
	SAFE_DELETE_ARRAY( mData );
}

int File::size() const {
	return mSize;
}

const char* File::data() const {
	return mData;
}

//unsignedæ‚èo‚µ
unsigned File::getUnsigned( int p ) const {
	const unsigned char* up;
	up = reinterpret_cast< const unsigned char* >( mData );
	unsigned r = up[ p ];
	r |= up[ p + 1 ] << 8;
	r |= up[ p + 2 ] << 16;
	r |= up[ p + 3 ] << 24;
	return r;
}
