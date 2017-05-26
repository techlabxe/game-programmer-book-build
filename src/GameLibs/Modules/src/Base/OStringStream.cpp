#include "GameLib/GameLib.h"
#include "GameLib/Base/OStringStream.h"
#include "GameLib/Base/RefString.h"
#include "GameLib/Base/Tank.h"
#include "GameLib/Base/Conversion.h"

namespace GameLib{

class OStringStream::Impl{
public:
	Impl( int blockSize ) : mBuffer( blockSize ),mIsHex( false ),mPrecision( 6 ){
	}
	~Impl(){
	}
	void enableHex( bool f ){
		mIsHex = f;
	}
	int getPrecision() const {
		return mPrecision;
	}
	void setPrecision( int a ){
		if ( a > 17 ){ 
			a = 17; //限界は17。doubleの10進精度がこれくらいになる
		}else if ( a < 1 ){
			a = 1;
		}
		mPrecision = a;
	}
	int size() const {
		return mBuffer.size();
	}
	void add( double a ){
		char s[ 30 ];
		int c = toString( s, a, mPrecision );
		ASSERT( c <= 30 );
		for ( int i = 0; i < c; ++i ){
			mBuffer.add( s[ i ] );
		}
	}
	void add( float a ){ //さてどうやってやろう。
		char s[ 16 ];
		int c = toString( s, a, mPrecision );
		ASSERT( c <= 16 );
		for ( int i = 0; i < c; ++i ){
			mBuffer.add( s[ i ] );
		}
	}
	void add( int a ){
		char s[ 11 ];
		int c = 0;
		if ( mIsHex ){
			c = toString16( s, a );
		}else{
			c = toString10( s, a );
		}
		ASSERT( c <= 11 );
		for ( int i = 0; i < c; ++i ){
			mBuffer.add( s[ i ] );
		}
	}
	void add( unsigned a ){
		char s[ 11 ];
		int c = 0;
		if ( mIsHex ){
			c = toString16( s, a );
		}else{
			c = toString10( s, a );
		}
		ASSERT( c <= 11 );
		for ( int i = 0; i < c; ++i ){
			mBuffer.add( s[ i ] );
		}
	}
	void add( short a ){
		char s[ 6 ];
		int c = 0;
		if ( mIsHex ){
			c = toString16( s, a );
		}else{
			c = toString10( s, a );
		}
		ASSERT( c <= 6 );
		for ( int i = 0; i < c; ++i ){
			mBuffer.add( s[ i ] );
		}
	}
	void add( unsigned short a ){
		char s[ 6 ];
		int c = 0;
		if ( mIsHex ){
			c = toString16( s, a );
		}else{
			c = toString10( s, a );
		}
		ASSERT( c <= 6 );
		for ( int i = 0; i < c; ++i ){
			mBuffer.add( s[ i ] );
		}
	}
	void add( unsigned char a ){
		char s[ 4 ];
		int c = 0;
		if ( mIsHex ){
			c = toString16( s, a );
		}else{
			c = toString10( s, a );
		}
		ASSERT( c <= 4 );
		for ( int i = 0; i < c; ++i ){
			mBuffer.add( s[ i ] );
		}
	}
	void add( char a ){
		if ( ( a == '\n' ) || ( a == '\t' ) ){
			mBuffer.add( a ); //変換せず(空白文字)
		}else if ( a >= 0x20 && a <= 0x7e ){
			mBuffer.add( a ); //変換せず
		}else{
			mBuffer.add( '%' );
			char s[ 2 ];
			int c = toString16( s, a ); //16進で
			ASSERT( c <= 2 );
			for ( int i = 0; i < c; ++i ){
				mBuffer.add( s[ i ] );
			}
		}
	}
	void add( const char* a ){
		while ( *a != '\0' ){
			mBuffer.add( *a );
			++a;
		}
	}
	void add( const string& a ){
		add( a.c_str() );
	}
	void add( const char* s, int n ){
		for ( int i = 0; i < n; ++i ){
			mBuffer.add( s[ i ] );
		}
	}
	void get( Array< char >* a ) const {
		a->setSize( mBuffer.size() + 1 );
		if ( mBuffer.size() > 0 ){
			mBuffer.copyTo( &( *a )[ 0 ] );
		}
		( *a )[ mBuffer.size() ] = '\0'; //NULL終端
	}
	void clear(){
		mBuffer.clear();
	}
private:
	Tank< char > mBuffer;
	bool mIsHex;
	int mPrecision;
};

OStringStream::OStringStream( int blockSize ) : mImpl( 0 ){
	mImpl = NEW Impl( blockSize );
}

OStringStream::~OStringStream(){
	SAFE_DELETE( mImpl );
}

OStringStream& OStringStream::operator<<( char a ){
	mImpl->add( a );
	return *this;
}

OStringStream& OStringStream::operator<<( unsigned char a ){
	mImpl->add( a );
	return *this;
}

OStringStream& OStringStream::operator<<( int a ){
	mImpl->add( a );
	return *this;
}

OStringStream& OStringStream::operator<<( unsigned a ){
	mImpl->add( a );
	return *this;
}

OStringStream& OStringStream::operator<<( short a ){
	mImpl->add( a );
	return *this;
}

OStringStream& OStringStream::operator<<( unsigned short a ){
	mImpl->add( a );
	return *this;
}

OStringStream& OStringStream::operator<<( float a ){
	mImpl->add( a );
	return *this;
}

OStringStream& OStringStream::operator<<( double a ){
	mImpl->add( a );
	return *this;
}

OStringStream& OStringStream::operator<<( const string& a ){
	mImpl->add( a );
	return *this;
}

OStringStream& OStringStream::operator<<( const char* s ){
	mImpl->add( s );
	return *this;
}

OStringStream& OStringStream::operator<<( char* s ){
	mImpl->add( s );
	return *this;
}

OStringStream& OStringStream::operator<<( const RefString& s ){
	mImpl->add( s.get(), s.size() );
	return *this;
}

void OStringStream::write( const char* s, int size ){
	mImpl->add( s, size );
}

OStringStream& OStringStream::endl(){
	mImpl->add( '\n' );
	return *this;
}

OStringStream& OStringStream::hex(){
	mImpl->enableHex( true );
	return *this;
}

OStringStream& OStringStream::dec(){
	mImpl->enableHex( false );
	return *this;
}

int OStringStream::precision( int a ){
	int r = mImpl->getPrecision();
	mImpl->setPrecision( a );
	return r;
}

OStringStream& OStringStream::operator<<( OStringStream& ( *f )( OStringStream& ) ){
	return ( *f )( *this );
}

void OStringStream::get( Array< char >* a ) const {
	mImpl->get( a );
}

void OStringStream::get( string* a ) const {
	if ( mImpl->size() > 0 ){
		Array< char > t;
		mImpl->get( &t );
		*a = string( &t[ 0 ], t.size() - 1 ); //ArraySizeはNULL終端分多いので引いてやる
	}
}

int OStringStream::size() const {
	return mImpl->size();
}

void OStringStream::clear(){
	mImpl->clear();
}

OStringStream& endl( OStringStream& a ){
	return a.endl();
}

OStringStream& hex( OStringStream& a ){
	return a.hex();
}

OStringStream& dec( OStringStream& a ){
	return a.dec();
}

} //namespace GameLib