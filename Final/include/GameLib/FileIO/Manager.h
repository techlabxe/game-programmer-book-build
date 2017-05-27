#ifndef INCLUDED_GAMELIB_FILEIO_MANAGER_H
#define INCLUDED_GAMELIB_FILEIO_MANAGER_H

#include <string>

namespace GameLib{
namespace FileIO{

class Manager{
public:
	//アーカイブの使い方
	enum AccessMode{
		MODE_ARCHIVE_FIRST, //アーカイブ優先。
		MODE_DIRECT_FIRST, //直接アクセス優先。
		MODE_ARCHIVE_ONLY, //アーカイブのみ。配る時にはこれにしておいた方がいい。
	};
	///インスタンスを得る。しかし、単にManager().foo()と書いても良い。
	static Manager instance();
	///ハンドルを受け取らずに書き込みを行う。成功失敗の別はわからない。
	void write( const char* filename, const char* data, int size );
	///ロードしているものをリストにして吐き出す。デバグ用。開放漏れを調べよう。
	std::string createListString() const;
	///容量限界の指定。warningを超えると警告が、errorを超えるとエラーにする。標準値は両方2GB
	void setLimit( int warning, int error );
	///エラーを起こした時に止まるかどうか。標準はtrue。ツール等止まって困るならfalseを入れよ。
	void enableHaltOnError( bool );

	//以下ユーザは意識しない
	static void create( 
		const char** archiveNames = 0, 
		int archiveNumber = 0, 
		AccessMode = MODE_DIRECT_FIRST );
	static void destroy();
};

} //namespace Sound
} //namespace GameLib

#endif
