#include "GameLib/GameLib.h"
#include "GameLib/WindowCreator/WindowCreator.h"
#include "DebugScreen.h"
#include "DebugScreenImpl.h"

namespace GameLib{

DebugScreenImpl* gDebugScreenImpl;

DebugScreen::DebugScreen(){
	//別スレッドからの呼び出しは許さない
	ASSERT( WindowCreator::isMainThread() && "you must call from MAIN thread" );
}

DebugScreen DebugScreen::instance(){
	return DebugScreen();
}

DebugScreen& DebugScreen::operator<<( char a ){
	gDebugScreenImpl->mBuffer << a;
	return *this;
}

DebugScreen& DebugScreen::operator<<( unsigned char a ){
	gDebugScreenImpl->mBuffer << a;
	return *this;
}

DebugScreen& DebugScreen::operator<<( int a ){
	gDebugScreenImpl->mBuffer << a;
	return *this;
}

DebugScreen& DebugScreen::operator<<( unsigned a ){
	gDebugScreenImpl->mBuffer << a;
	return *this;
}

DebugScreen& DebugScreen::operator<<( short a ){
	gDebugScreenImpl->mBuffer << a;
	return *this;
}

DebugScreen& DebugScreen::operator<<( unsigned short a ){
	gDebugScreenImpl->mBuffer << a;
	return *this;
}

DebugScreen& DebugScreen::operator<<( float a ){
	gDebugScreenImpl->mBuffer << a;
	return *this;
}

DebugScreen& DebugScreen::operator<<( double a ){
	gDebugScreenImpl->mBuffer << a;
	return *this;
}

DebugScreen& DebugScreen::operator<<( const string& a ){
	gDebugScreenImpl->mBuffer << a;
	return *this;
}

DebugScreen& DebugScreen::operator<<( const char* s ){
	gDebugScreenImpl->mBuffer << s;
	return *this;
}

DebugScreen& DebugScreen::operator<<( char* s ){
	gDebugScreenImpl->mBuffer << s;
	return *this;
}

DebugScreen& DebugScreen::endl(){
	gDebugScreenImpl->add();
	gDebugScreenImpl->newLine();
	return *this;
}

DebugScreen& DebugScreen::hex(){
	gDebugScreenImpl->mBuffer.hex();
	return *this;
}

DebugScreen& DebugScreen::dec(){
	gDebugScreenImpl->mBuffer.dec();
	return *this;
}

int DebugScreen::precision( int a ){
	int r = gDebugScreenImpl->mBuffer.precision( a );
	return r;
}

DebugScreen& DebugScreen::setColor( unsigned c ){
	if ( ( c & 0xff000000 ) == 0 ){
		c |= 0xff000000;
	}
	gDebugScreenImpl->add();
	gDebugScreenImpl->mColor = c;
	return *this;
}

DebugScreen& DebugScreen::operator<<( const Color& c ){
	return setColor( c.mColor );
}

DebugScreen& DebugScreen::setPosition( int x, int y ){
	gDebugScreenImpl->add();
	gDebugScreenImpl->setPosition( x, y );
	return *this;
}

void DebugScreen::draw( int x, int y, const char* s, unsigned c ){
	if ( ( c & 0xff000000 ) == 0 ){
		c |= 0xff000000;
	}
	gDebugScreenImpl->mRenderer.add( x * 8, y * 16, s, c );
}

DebugScreen& DebugScreen::operator<<( DebugScreen& ( *f )( DebugScreen& ) ){
	return ( *f )( *this );
}

DebugScreen& endl( DebugScreen& a ){
	return a.endl();
}

DebugScreen& hex( DebugScreen& a ){
	return a.hex();
}

DebugScreen& dec( DebugScreen& a ){
	return a.dec();
}


} //namespace GameLib
