#ifndef INCLUDED_LOADER_H
#define INCLUDED_LOADDR_H

#include <list>
#include <string>
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
	static Loader* mInstance;
};

#endif
