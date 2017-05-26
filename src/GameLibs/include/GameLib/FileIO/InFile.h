#ifndef INCLUDED_GAMELIB_FILEIO_INFILE_H
#define INCLUDED_GAMELIB_FILEIO_INFILE_H

namespace GameLib{
namespace FileIO{

///読み込みファイルハンドル
class InFile{
public:
	///ファイルロードを開始する
	static InFile create( const char* filename );
	///ロード容量ゲット
	int size() const;
	///ロード内容ゲット
	const char* data() const; 
	///ロード終わった？
	bool isFinished() const;
	///エラー出てます？
	bool isError() const;
	//値ゲット関数群
	int getInt( int position ) const;
	unsigned getUnsigned( int position ) const;
	short getShort( int position ) const;
	unsigned short getUnsignedShort( int position ) const;

	//以下ユーザは意識しない関数群
	InFile();
	InFile( const InFile& );
	~InFile();
	operator void*() const;
	InFile& operator=( const InFile& );
	void release();
	bool operator==( const InFile& ) const;
	bool operator!=( const InFile& ) const;

	class Impl;
private:
	Impl* mImpl;
};

} //namespace FileIO
} //namespace GameLib

#endif
