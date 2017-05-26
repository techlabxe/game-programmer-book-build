#include "GameLib/Base/IBinaryStream.h"

namespace GameLib{
using namespace std;

IBinaryStream::IBinaryStream( const char* s ) : mBuffer( s ), mSize( 0 ), mPosition( 0 ){
	if ( s ){
		while ( *s ){
			++mSize;
			++s;
		}
	}
}

IBinaryStream::IBinaryStream( const char* s, int size ) : mBuffer( s ), mSize( size ), mPosition( 0 ){
}

IBinaryStream::IBinaryStream( const RefString& s ) : 
mBuffer( s.get() ), 
mSize( s.size() ), 
mPosition( 0 ){
}

IBinaryStream::~IBinaryStream(){
	mBuffer = 0;
	mSize = mPosition = 0;
}

int IBinaryStream::read( char* dst, int size ){
	if ( size > mSize - mPosition ){
		size = mSize - mPosition;
	}
	for ( int i = 0; i < size; ++i ){
		dst[ i ] = mBuffer[ mPosition + i ];
	}
	mPosition += size;
	return size;
}

int IBinaryStream::readInt( bool* succeeded ){
	if ( mPosition + 4 < mSize ){
		if ( succeeded ){
			*succeeded = true;
		}
	}else{
		if ( succeeded ){
			*succeeded = false;
		}
	}
	const unsigned char* p;
	p = reinterpret_cast< const unsigned char* >( mBuffer + mPosition );

	int r = p[ 0 ];
	r += p[ 1 ] << 8;
	r += p[ 2 ] << 16;
	r += p[ 3 ] << 24;
	mPosition += 4;
	return r;
}

unsigned IBinaryStream::readUnsigned( bool* succeeded ){
	if ( mPosition + 4 < mSize ){
		if ( succeeded ){
			*succeeded = true;
		}
	}else{
		if ( succeeded ){
			*succeeded = false;
		}
		return 0;
	}
	const unsigned char* p;
	p = reinterpret_cast< const unsigned char* >( mBuffer + mPosition );

	unsigned r = p[ 0 ];
	r += p[ 1 ] << 8;
	r += p[ 2 ] << 16;
	r += p[ 3 ] << 24;
	mPosition += 4;
	return r;
}

short IBinaryStream::readShort( bool* succeeded ){
	if ( mPosition + 2 < mSize ){
		if ( succeeded ){
			*succeeded = true;
		}
	}else{
		if ( succeeded ){
			*succeeded = false;
		}
		return 0;
	}
	const unsigned char* p;
	p = reinterpret_cast< const unsigned char* >( mBuffer + mPosition );

	short r = p[ 0 ];
	r += p[ 1 ] << 8;
	mPosition += 2;
	return r;
}

unsigned short IBinaryStream::readUnsignedShort( bool* succeeded ){
	if ( mPosition + 2 < mSize ){
		if ( succeeded ){
			*succeeded = true;
		}
	}else{
		if ( succeeded ){
			*succeeded = false;
		}
		return 0;
	}
	const unsigned char* p;
	p = reinterpret_cast< const unsigned char* >( mBuffer + mPosition );

	unsigned short r = p[ 0 ];
	r += p[ 1 ] << 8;
	mPosition += 2;
	return r;
}

char IBinaryStream::readChar( bool* succeeded ){
	if ( mPosition + 1 < mSize ){
		if ( succeeded ){
			*succeeded = true;
		}
	}else{
		if ( succeeded ){
			*succeeded = false;
		}
		return 0;
	}
	++mPosition;
	return mBuffer[ mPosition - 1 ];
}

unsigned char IBinaryStream::readUnsignedChar( bool* succeeded ){
	if ( mPosition + 1 < mSize ){
		if ( succeeded ){
			*succeeded = true;
		}
	}else{
		if ( succeeded ){
			*succeeded = false;
		}
		return 0;
	}
	const unsigned char* p;
	p = reinterpret_cast< const unsigned char* >( mBuffer + mPosition );
	++mPosition;
	return *p;
}

bool IBinaryStream::isEnd() const {
	return ( mPosition >= mSize );
}

int IBinaryStream::position() const {
	return mPosition;
}

int IBinaryStream::size() const {
	return mSize;
}

void IBinaryStream::setPosition( int p ){
	mPosition = p;
}

} //namespace GameLib

