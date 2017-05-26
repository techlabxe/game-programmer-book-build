#ifndef INCLUDED_LOADER_H
#define INCLUDED_LOADDR_H

#include <list>
#include <string>
#include <fstream>
#include "GameLib/Threading/Thread.h"
#include "GameLib/Threading/Mutex.h"
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

//Loader::updateを呼ぶだけの人
class LoadingThread : public GameLib::Threading::Thread{
public:
	LoadingThread(){ start(); }
	~LoadingThread(){ wait(); }
	void operator()();
};

class Loader{
public:
	static Loader* instance();
	static void create();
	static void destroy();

	void createFile( File**, const char* filename );
	void destroyFile( File** );
	void update( bool* endOut ); //終わる時はendOutにtrueを入れる
private:
	Loader();
	Loader( const Loader& ); //封印
	~Loader();

	//スレッドの都合により配列にする。
	static const int MAX_FILE_NUMBER = 100;
	File* mFiles[ MAX_FILE_NUMBER ];
	
	LoadingThread* mThread; //スレッド
	GameLib::Threading::Mutex mLock;
	bool mEndRequest; //終わるときにtrueにする。
	static const int READ_UNIT = 1 * 1024 * 1024; //1MB

	static Loader* mInstance;
};

#endif
