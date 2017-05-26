#include "BitStream.h"
#include <cassert>

IBitStream::IBitStream( const unsigned char* buffer ) : mBuffer( buffer ), mPosition( 0 ){
}

void IBitStream::read( unsigned char* data, int sizeInBit ){
	for ( int i = 0; i < sizeInBit; ++i ){
		int dst = i;
		int src = mPosition + i;
		int dstIndex = dst / 8;
		int dstShift = dst % 8;
		int srcIndex = src / 8;
		int srcShift = src % 8;
		if ( mBuffer[ srcIndex ] & ( 1 << srcShift ) ){
			data[ dstIndex ] |= ( 1 << dstShift );
		}else{
			data[ dstIndex ] &= ~( 1 << dstShift );
		}
	}
	mPosition += sizeInBit;
}

bool IBitStream::read(){
	int src = mPosition;
	int srcIndex = src / 8;
	int srcShift = src % 8;
	bool r = ( mBuffer[ srcIndex ] & ( 1 << srcShift ) ) ? true : false;
	++mPosition;
	return r;
}

unsigned IBitStream::readU4(){
	unsigned char t[ 4 ];
	read( t, 32 );
	unsigned r = t[ 0 ];
	r |= ( t[ 1 ] << 8 );
	r |= ( t[ 2 ] << 16 );
	r |= ( t[ 3 ] << 24 );
	return r;
}

int IBitStream::readS4(){
	return static_cast< int >( readU4() );
}

int IBitStream::positionInByte() const {
	return mPosition / 8;
}


OBitStream::OBitStream( unsigned char* buffer ) : mBuffer( buffer ), mPosition( 0 ){
}

void OBitStream::write( const unsigned char* data, int sizeInBit ){
	for ( int i = 0; i < sizeInBit; ++i ){
		int dst = mPosition + i;
		int src = i;
		int dstIndex = dst / 8;
		int dstShift = dst % 8;
		int srcIndex = src / 8;
		int srcShift = src % 8;
		if ( data[ srcIndex ] & ( 1 << srcShift ) ){
			mBuffer[ dstIndex ] |= ( 1 << dstShift );
		}else{
			mBuffer[ dstIndex ] &= ~( 1 << dstShift );
		}
	}
	mPosition += sizeInBit;
}

void OBitStream::write( bool f ){
	int dst = mPosition;
	int dstIndex = dst / 8;
	int dstShift = dst % 8;
	if ( f ){
		mBuffer[ dstIndex ] |= ( 1 << dstShift );
	}else{
		mBuffer[ dstIndex ] &= ~( 1 << dstShift );
	}
	++mPosition;
}

void OBitStream::write( unsigned a ){
	unsigned char t[ 4 ];
	t[ 0 ] = static_cast< unsigned char >( ( a & 0x000000ff ) >> 0 );
	t[ 1 ] = static_cast< unsigned char >( ( a & 0x0000ff00 ) >> 8 );
	t[ 2 ] = static_cast< unsigned char >( ( a & 0x00ff0000 ) >> 16 );
	t[ 3 ] = static_cast< unsigned char >( ( a & 0xff000000 ) >> 24 );
	write( t, 32 );
}

void OBitStream::write( int a ){
	write( static_cast< unsigned >( a ) );
}

void OBitStream::add(){
	int i = mPosition;
	do{
		--i; //先に1個戻る
		int index = i / 8;
		int shift = i % 8;
		if ( mBuffer[ index ] & ( 1 << shift ) ){ //キャリーします。
			mBuffer[ index ] &= ~( 1 << shift );
		}else{
			mBuffer[ index ] |= ( 1 << shift );
			break;
		}
	}while ( i >= 0 );
	assert( i >= 0 );
}

int OBitStream::sizeInByte() const {
	return ( mPosition + 7 ) / 8; //8で割って余りが0ならそのまま、余りがあれば+1して返すのだが、7足してから8で割ると同じことができる。
}

