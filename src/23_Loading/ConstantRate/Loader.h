#ifndef INCLUDED_LOADER_H
#define INCLUDED_LOADDR_H

#include <list>
#include <string>
#include <fstream>
using namespace std;

class File{
public:
	bool isReady() const; //ロード終わってる？
	int getSize() const; //ファイルサイズ取得
	const char* getData() const; //中身取得
private:
	File( const char* filename );
	~File();
	friend class Loader;

	string mFilename;
	char* mData;
	int mSize;
};

class Loader{
public:
	static Loader* instance();
	static void create();
	static void destroy();

	void createFile( File**, const char* filename );
	void destroyFile( File** );
	void update();
private:
	Loader();
	Loader( const Loader& ); //封印
	~Loader();

	list< File* > mFiles;
	
	File* mCurrentFile; //今処理しているFile
	ifstream* mStream; //読んでるファイル
	int mFileSize; //現在読み込み中のファイルのサイズ
	int mFilePosition; //現在読み込み中のファイルの場所
	char* mData; //読み込み中バッファ
	static const int READ_UNIT = 128 * 1024; //128KB

	static Loader* mInstance;
};

#endif
