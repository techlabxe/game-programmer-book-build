#ifndef INCLUDED_GAMELIB_IBINARYSTREAM_H
#define INCLUDED_GAMELIB_IBINARYSTREAM_H

#include "GameLib/Base/RefString.h"

namespace GameLib{

class RefString;

///バイナリファイルの読み込みを楽にするためのクラス
/*!
基本的にコピーは行わない。したがって、元の文字列は保持されている必要がある。
*/
class IBinaryStream{
public:
	IBinaryStream( const char* );
	IBinaryStream( const char*, int size );
	IBinaryStream( const RefString& );
	~IBinaryStream();
	///戻り値は読めたバイト数
	int read( char*, int size );
	///各種組み込み整数型用のバージョン。成功失敗は引数に返るが、渡さなければ何もしない。
	int readInt( bool* succeeded = 0 );
	unsigned readUnsigned( bool* succeeded = 0 );
	short readShort( bool* succeeded = 0 );
	unsigned short readUnsignedShort( bool* succeeded = 0 );
	char readChar( bool* succeeded = 0 );
	unsigned char readUnsignedChar( bool* succeeded = 0 );

	bool isEnd() const;
	void setPosition( int = 0 );
	int position() const;
	int size() const;
private:
	const char* mBuffer;
	int mSize;
	int mPosition;
};

} //namespace GameLib

#endif