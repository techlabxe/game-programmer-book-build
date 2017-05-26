#include "GameLib/Framework.h"
#include "GameLib/DebugScreen.h"
#include "GameLib/Math/Random.h"
#include "GameLib/Threading/Thread.h"
using namespace std;

using namespace GameLib::Math;
const int N = 10;
const int M = 100;
Random gRandom[ N ];

typedef unsigned char U1;
U1* p[ N ][ M ];
int gCount;

class MyThread : GameLib::Threading::Thread{
public:
	MyThread( int id ) : mId( id ){
		start();
	}
	~MyThread(){
		wait();
	}
	void operator()(){
		U1** q = p[ mId ];
		for ( int i = 0; i < N; ++i ){
			int n = gRandom[ mId ].getInt( M );
			if ( q[ n ] ){
				SAFE_DELETE_ARRAY( q[ n ] );
			}
			int size = 0;
			for ( int j = 0; j < 3; ++j ){
				size += ( 1 << gRandom[ mId ].getInt( 0, 18 ) );
			}
			q[ n ] = NEW U1[ size ];
			for ( int j = 0; j < size; ++j ){
#ifndef NDEBUG
				ASSERT( q[ n ][ j ] == 0xfb );
#endif
				q[ n ][ j ] = 0x55;
			}
		}
	}
	int mId;
};

namespace GameLib{
	void Framework::configure( Configuration* c ){
		c->setExtraThreadNumber( N );
	}
	void Framework::update(){
		if ( !gRandom[ 0 ] ){
			for ( int i = 0; i < N; ++i ){
				gRandom[ i ] = Random::create( i );
			}
		}
		//スレッド生成
		Array< MyThread* > threads( N );
		for ( int i = 0; i < N; ++i ){
			threads[ i ] = NEW MyThread( i );
		}
		//終了待ち
		for ( int i = 0; i < N; ++i ){
			SAFE_DELETE( threads[ i ] );
		}

		if ( gCount++ % 200 == 0 ){
			MemoryManager::instance().write();
		}

		DebugScreen() << frameRate();	
		if ( isEndRequested() ){
			for ( int j = 0; j < N; ++j ){
				for ( int i = 0; i < M; ++i ){
					SAFE_DELETE_ARRAY( p[ j ][ i ] );
				}
				gRandom[ j ].release();
			}
		}
	}
}
