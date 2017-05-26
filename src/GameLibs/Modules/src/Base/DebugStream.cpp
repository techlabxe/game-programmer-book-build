#include <windows.h>
#undef min
#undef max

#include "GameLib/GameLib.h"
#include "GameLib/Base/DebugStream.h"
#include "GameLib/Threading/Mutex.h"
#include "GameLib/Base/RefString.h"
#include "GameLib/Base/OStringStream.h"

namespace GameLib{
using namespace GameLib::Threading;

DebugStream cout;

class DebugStream::Impl{
public:
	Impl(){
		mMutex = Mutex::create();
	}
	OStringStream mBuffer;
	Mutex mMutex;
};

DebugStream::DebugStream() : mImpl( 0 ){
}

void DebugStream::begin(){
	mImpl = NEW Impl;
}

void DebugStream::end(){
	SAFE_DELETE( mImpl );
}

DebugStream& DebugStream::operator<<( char a ){
	mImpl->mMutex.lock();
	mImpl->mBuffer << a;
	mImpl->mMutex.unlock();
	return *this;
}

DebugStream& DebugStream::operator<<( unsigned char a ){
	mImpl->mMutex.lock();
	mImpl->mBuffer << a;
	mImpl->mMutex.unlock();
	return *this;
}

DebugStream& DebugStream::operator<<( int a ){
	mImpl->mMutex.lock();
	mImpl->mBuffer << a;
	mImpl->mMutex.unlock();
	return *this;
}

DebugStream& DebugStream::operator<<( unsigned a ){
	mImpl->mMutex.lock();
	mImpl->mBuffer << a;
	mImpl->mMutex.unlock();
	return *this;
}

DebugStream& DebugStream::operator<<( short a ){
	mImpl->mMutex.lock();
	mImpl->mBuffer << a;
	mImpl->mMutex.unlock();
	return *this;
}

DebugStream& DebugStream::operator<<( unsigned short a ){
	mImpl->mMutex.lock();
	mImpl->mBuffer << a;
	mImpl->mMutex.unlock();
	return *this;
}

DebugStream& DebugStream::operator<<( float a ){
	mImpl->mMutex.lock();
	mImpl->mBuffer << a;
	mImpl->mMutex.unlock();
	return *this;
}

DebugStream& DebugStream::operator<<( double a ){
	mImpl->mMutex.lock();
	mImpl->mBuffer << a;
	mImpl->mMutex.unlock();
	return *this;
}

/*
DebugStream& DebugStream::operator<<( const string& a ){
	mImpl->mMutex.lock();
	mImpl->mBuffer << a;
	mImpl->mMutex.unlock();
	return *this;
}
*/

DebugStream& DebugStream::operator<<( const char* s ){
	mImpl->mMutex.lock();
	mImpl->mBuffer << s;
	mImpl->mMutex.unlock();
	return *this;
}

DebugStream& DebugStream::operator<<( char* s ){
	mImpl->mMutex.lock();
	mImpl->mBuffer << s;
	mImpl->mMutex.unlock();
	return *this;
}

DebugStream& DebugStream::operator<<( const RefString& s ){
	mImpl->mMutex.lock();
	mImpl->mBuffer.write( s.get(), s.size() );
	mImpl->mMutex.unlock();
	return *this;
}

DebugStream& DebugStream::operator<<( const OStringStream& s ){
	if ( s.size() > 0 ){
		Array< char > a;
		s.get( &a );
		mImpl->mMutex.lock();
		mImpl->mBuffer << &a[ 0 ];
		mImpl->mMutex.unlock();
	}
	return *this;
}

DebugStream& DebugStream::endl(){
	mImpl->mMutex.lock();
	mImpl->mBuffer << '\n';
	Array< char > a;
	mImpl->mBuffer.get( &a );
	OutputDebugStringA( &a[ 0 ] ); //アスキー版直接呼出し
	mImpl->mBuffer.clear();
	mImpl->mMutex.unlock();
	return *this;
}

DebugStream& DebugStream::hex(){
	mImpl->mMutex.lock();
	mImpl->mBuffer.hex();
	mImpl->mMutex.unlock();
	return *this;
}

DebugStream& DebugStream::dec(){
	mImpl->mMutex.lock();
	mImpl->mBuffer.dec();
	mImpl->mMutex.unlock();
	return *this;
}

int DebugStream::precision( int a ){
	mImpl->mMutex.lock();
	int r = mImpl->mBuffer.precision( a );
	mImpl->mMutex.unlock();
	return r;
}

DebugStream& DebugStream::operator<<( DebugStream& ( *f )( DebugStream& ) ){
	return ( *f )( *this );
}

DebugStream& endl( DebugStream& a ){
	return a.endl();
}

DebugStream& hex( DebugStream& a ){
	return a.hex();
}

DebugStream& dec( DebugStream& a ){
	return a.dec();
}

} //namespace GameLib