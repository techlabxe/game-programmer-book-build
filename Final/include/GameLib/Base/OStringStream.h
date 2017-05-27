#ifndef INCLUDED_GAMELIB_OSTRINGSTREAM_H
#define INCLUDED_GAMELIB_OSTRINGSTREAM_H

#include <string>

namespace GameLib{
using namespace std;

class RefString;

///std::ostringstreamを置換するための物。速度に重点を置き、多少機能が犠牲になっている。
/*!
std::ostringstreamとの相違点は、
・str()がない。get()でArrayを渡して取り出す。
・clear()で全消去
・機能が少ない
*/
class OStringStream{
public:
	OStringStream( int blockSize = 1024 ); //一回のnewサイズ。大きいほど負荷は軽いが、メモリは食う。
	~OStringStream();
	int precision( int );
	OStringStream& operator<<( char );
	OStringStream& operator<<( unsigned char );
	OStringStream& operator<<( int );
	OStringStream& operator<<( unsigned );
	OStringStream& operator<<( short );
	OStringStream& operator<<( unsigned short );
	OStringStream& operator<<( float );
	OStringStream& operator<<( double );
	OStringStream& operator<<( const string& );
	OStringStream& operator<<( const char* );
	OStringStream& operator<<( char* );
	OStringStream& operator<<( const RefString& );
	OStringStream& operator<<( OStringStream& ( *f )( OStringStream& ) );
	void write( const char*, int size );
	template< class T > void write( const T& ); //バイナリ書き込み
	OStringStream& endl();
	OStringStream& hex();
	OStringStream& dec();

	void get( Array< char >* ) const;
	void get( string* ) const;
	int size() const;
	void clear();
private:
	OStringStream( const OStringStream& ); //コピーコンストラクタ禁止
	void operator=( const OStringStream& ); //代入禁止
	class Impl;
	Impl* mImpl;
};

OStringStream& endl( OStringStream& );
OStringStream& hex( OStringStream& );
OStringStream& dec( OStringStream& );

} //namespace GameLib
#include "GameLib/Base/Impl/OStringStreamImpl.h"

#endif
