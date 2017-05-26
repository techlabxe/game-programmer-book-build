#include "GameLib/GameLib.h"
#include "GameLib/Base/RefString.h"

namespace GameLib{

RefString::RefString() : mString( 0 ), mSize( 0 ){
}

RefString::RefString( const char* s, int size ) : mString( s ), mSize( size ){
}

RefString::RefString( const char* s ) : mString( s ), mSize( 0 ){
	while ( *s ){
		++mSize;
		++s;
	}
}

RefString::~RefString(){
	mString = 0;
}

void RefString::set( const char* s, int size ){
	mString = s;
	mSize = size;
}

void RefString::operator=( const char* s ){
	mString = s;
	mSize = 0;
	if ( s ){
		while ( *s ){
			++mSize;
			++s;
		}
	}
}

char RefString::operator[]( int i ) const {
	ASSERT( i >= 0 && i < mSize );
	return mString[ i ];
}

const char* RefString::get() const {
	return mString;
}

int RefString::size() const {
	return mSize;
}

void RefString::setSize( int size ){
	mSize = size;
}

void RefString::clamp( int first, int last ){
	ASSERT( first < mSize );
	ASSERT( last < mSize && first < last );
	if ( first < 0 ){
		first = 0;
	}
	if ( last < 0 ){
		last = mSize - 1;
	}
	mString = mString + first;
	mSize = last - first + 1; //+1が必要。lastも含まれているからだ
}

int RefString::find( char c ) const {
	for ( int i = 0; i < mSize; ++i ){
		if ( mString[ i ] == c ){
			return i;
		}
	}
	return -1;
}

int RefString::rfind( char c ) const {
	for ( int i = mSize - 1; i >= 0; --i ){
		if ( mString[ i ] == c ){
			return i;
		}
	}
	return -1;
}
/*
void RefString::operator=( const string& s ){
	mString = s.c_str();
	mSize = static_cast< int >( s.size() );
}
*/
bool RefString::operator==( const RefString& s ) const {
	if ( mSize != s.mSize ){
		return false;
	}
	for ( int i = 0; i < mSize; ++i ){
		if ( mString[ i ] != s.mString[ i ] ){
			return false;
		}
	}
	return true;
}

bool RefString::operator==( const char* s ) const {
	int i = 0;
	while ( ( i < mSize ) && s[ i ] ){
		if ( mString[ i ] != s[ i ] ){
			return false;
		}
		++i;
	}
	return ( ( s[ i ] == '\0' ) && ( i == mSize ) ); //同時に終わったらtrue
}
/*
bool RefString::operator==( const string& s ) const {
	if ( mSize != static_cast< int >( s.size() ) ){
		return false;
	}
	for ( int i = 0; i < mSize; ++i ){
		if ( mString[ i ] != s[ i ] ){
			return false;
		}
	}
	return true;
}
*/
bool RefString::operator!=( const RefString& s ) const {
	return !operator==( s );
}

bool RefString::operator!=( const char* s ) const {
	return !operator==( s );
}

/*
bool RefString::operator!=( const string& s ) const {
	return !operator==( s );
}
*/
bool RefString::operator<( const RefString& s ) const {
	bool f; //どっちかが終わるまでに決着が付かなかった場合、文字数が少ない方が小さい
	int n;
	if ( mSize < s.mSize ){
		f = true;
		n = mSize;
	}else{
		f = false;
		n = s.mSize;
	}
	for ( int i = 0; i < n; ++i ){
		if ( mString[ i ] < s.mString[ i ] ){
			return true;
		}else if ( mString[ i ] > s.mString[ i ] ){
			return false;
		}
	}
	return f;
}

char* RefString::copyOriginal() const {
	char* r = NEW char[ mSize + 1 ];
	for ( int i = 0; i < mSize; ++i ){
		r[ i ] = mString[ i ];
	}
	r[ mSize ] = '\0';
	return r;
}

} //namespace GameLib
