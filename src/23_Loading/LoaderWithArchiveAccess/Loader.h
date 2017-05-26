#ifndef INCLUDED_LOADER_H
#define INCLUDED_LOADDR_H

#include <list>
#include <string>
#include <map>
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
	friend class Archive;

	string mFilename;
	char* mData;
	int mSize;
};

//アーカイブクラス
class Archive{
public:
	Archive( const char* archiveName );
	~Archive();
	void read( File* ) ;
private:
	struct Entry{
		int mPosition;
		int mSize;
	};
	int mFileNumber;
	ifstream* mStream;
	map< string, Entry > mEntries;
};

class Loader{
public:
	static Loader* instance();
	//アーカイブから読むならアーカイブ名を渡せ
	static void create( const char* archiveName = 0 );
	static void destroy();

	void createFile( File**, const char* filename );
	void destroyFile( File** );
	void update();
private:
	Loader( const char* filename );
	Loader( const Loader& ); //封印
	~Loader();

	list< File* > mFiles;
	Archive* mArchive;

	static Loader* mInstance;
};

#endif
