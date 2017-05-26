#ifndef INCLUDED_GAMELIB_DEBUGSCREEN_H
#define INCLUDED_GAMELIB_DEBUGSCREEN_H

#include <string>

namespace GameLib{
using namespace std;

class DebugScreen{
public:
	struct Color{
		Color( unsigned c ) : mColor( c ){}
		unsigned mColor;
	};
	DebugScreen();
	static DebugScreen instance();

	int precision( int );
	DebugScreen& operator<<( char );
	DebugScreen& operator<<( unsigned char );
	DebugScreen& operator<<( int );
	DebugScreen& operator<<( unsigned );
	DebugScreen& operator<<( short );
	DebugScreen& operator<<( unsigned short );
	DebugScreen& operator<<( float );
	DebugScreen& operator<<( double );
	DebugScreen& operator<<( const string& );
	DebugScreen& operator<<( const char* );
	DebugScreen& operator<<( char* );
	DebugScreen& operator<<( const Color& );
	DebugScreen& operator<<( DebugScreen& ( *f )( DebugScreen& ) );
	DebugScreen& endl();
	DebugScreen& hex();
	DebugScreen& dec();
	//色を変更する。アルファが0だと勝手に255にするので、0xff4477のように渡せて楽
	DebugScreen& setColor( unsigned );
	//位置を移動させる。rowは行頭の位置を表し、改行するとrowから始まる。
	DebugScreen& setPosition( int row, int column );
	//色、位置の現在値をいじらずに単発で描画
	void draw( int col, int row, const char* s, unsigned color = 0xffffffff );
};

DebugScreen& endl( DebugScreen& );
DebugScreen& hex( DebugScreen& );
DebugScreen& dec( DebugScreen& );

} //namespace GameLib

#endif
