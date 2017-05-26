#include "GameLib/Framework.h"
#include "GameLib/Threading/Thread.h"
using namespace GameLib;
using namespace GameLib::Threading;

//スレッド派生
class MyThread : public Thread{
public:
	void operator()(){
		cout << "I am a thread." << endl;
	}
};

MyThread* gThread;

namespace GameLib{
	void Framework::update(){
		if ( !gThread ){
			//スレッド生成、開始
			gThread = new MyThread();
			gThread->start();
		}
		if ( isEndRequested() ){
			gThread->wait(); //終わってから消すこと。
			SAFE_DELETE( gThread );
		}
	}
} //namespace GameLib
