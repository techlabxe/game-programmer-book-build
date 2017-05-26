#include "GameLib/Framework.h"
#include "GameLib/Threading/Thread.h"
#include "GameLib/Threading/Mutex.h"
#include <sstream>
using namespace std;
using namespace GameLib::Threading;

class Bool{
public:
	Bool() : mBool( false ){
		mMutex = Mutex::create();
	}
	void operator=( bool f ){
		mMutex.lock();
		mBool = f;
		mMutex.unlock();
	}
	bool get(){
		//一見奇妙に見えるだろうが、読む時もlock(),unlock()しておく。キャッシュの問題と、コンパイラのおせっかいを抑制するためだ。
		mMutex.lock();
		bool r = mBool;
		mMutex.unlock();
		return r;
	}
private:
	bool mBool;
	Mutex mMutex;
};

int gX;
Bool gWrite;
Bool gRead;

//スレッドで実行する関数を持つクラス
class MyThread : public Thread{
public:
	MyThread(){ start(); }
	~MyThread(){ wait(); }
	void operator()(){
		for ( int i = 0; i < 100; ++i ){
			while ( !gRead.get() ){ //読まれるのを待つ
				;
			}
			gX += 2;
			gRead = false; //まだ読まれてない
			gWrite = true; //書きました
		}
	}
};

namespace GameLib{
	void Framework::update(){
		setFrameRate( 60 );
		ostringstream o;
		gX = 0;
		gWrite = false;
		gRead = true;
		MyThread t;

		for ( int i = 0; i < 100; ++i ){
			while ( !gWrite.get() ){ //書かれるのを待つ
				;
			}
			o.str( "" );
			o << gX;
			gWrite = false;
			gRead = true;
			drawDebugString( ( i % 16 ) * 4, i / 16, o.str().c_str() );
		}
	}
} //namespace GameLib
