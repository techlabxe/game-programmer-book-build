#include "GameLib/Framework.h"
#include "GameLib/DebugScreen.h"
#include "GameLib/Threading/Thread.h"
#include "GameLib/Math/Random.h"
using namespace std;
using namespace GameLib;
using namespace GameLib::Threading;
using namespace GameLib::Math;

Random gRandom;

class MyThread : public Thread{
public:
	MyThread( int x ) : mX( x ){
		start();
	}
	~MyThread(){
		wait();
	}
	void operator()(){
		int j = 0;
		for ( int i = 0; i < mX; ++i ){
			++j;
		}
	}
private:
	int mX;
};

const int N = 1000;
MyThread* gThreads[ N ];

namespace GameLib{
	void Framework::configure( Configuration* c ){
		c->setExtraThreadNumber( 1 );
	}
	void Framework::update(){
		if ( !gRandom ){
			gRandom = Random::create();
		}
		int n = gRandom.getInt( N );
		for ( int i = 0; i < N; ++i ){
			int j = n + i;
			j = ( j >= N ) ? ( j - N ) : j;
			//I‚í‚Á‚Ä‚é‚Ì‚ð’T‚·
			if ( !gThreads[ j ] || gThreads[ j ]->isFinished() ){
				SAFE_DELETE( gThreads[ j ] );
				gThreads[ j ] = new MyThread( gRandom.getInt( 0x1ffffff ) );
			}
		}
		if ( isEndRequested() ){
			for ( int i = 0; i < N; ++i ){
				SAFE_DELETE( gThreads[ i ] );
			}
		}
		DebugScreen() << frameRate();
	}
}
