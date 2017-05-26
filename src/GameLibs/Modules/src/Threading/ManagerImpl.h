#ifndef INCLUDED_GAMELIB_THREADING_MANAGERIMPL_H
#define INCLUDED_GAMELIB_THREADING_MANAGERIMPL_H

#include <windows.h>
#include <process.h>
#undef min
#undef max
#include <list>

#include "GameLib/GameLib.h"
#include "GameLib/Threading/Manager.h"
#include "GameLib/Threading/Thread.h"
#include "GameLib/Threading/Event.h"
#include "GameLib/Threading/Mutex.h"
#include "GameLib/Threading/Semaphore.h"

namespace GameLib{
namespace Threading{
using namespace std;

class ManagerImpl{
public:
	struct Entry{
		Entry() : mThread( 0 ), mFinished( 0 ){}
		Entry( Thread* t, Event* e ) : mThread( t ), mFinished( e ){}
		~Entry(){ 
			mThread = 0;
			mFinished = 0;
		}
		Thread* mThread;
		Event* mFinished;
	};
	ManagerImpl( int additionalThreadNumber ) : 
	mThreads( 0 ), 
	mThreadNumber( 0 ), 
	mEndRequest( false ),
	mCoreNumber( 1 ){ //最低でも1
		mQueueSemaphore = Semaphore::create( 0, 0x7fffffff ); //最大値は無限
		mLock = Mutex::create();

		//コア数取得
#ifdef NDEBUG //Debug版では自動では一個しか作らない。開発マシンで動いて他で動かない、を防ぐため。
		HANDLE process = GetCurrentProcess();
		DWORD processMask;
		DWORD systemMask;
		BOOL succeeded = GetProcessAffinityMask( process, &processMask, &systemMask );
		if ( succeeded != 0 ){ //失敗されても困るけどな...
			for ( int i = 1; i < 32; ++i ){
				if ( processMask & ( 1 << i ) ){
					++mCoreNumber;
				}
			}
		}
#endif
		mThreadNumber = additionalThreadNumber + mCoreNumber + 1; //最低でもコア数+1(ローダ用)は作る
		mThreads = new HANDLE[ mThreadNumber ];
		//起動
		for ( int i = 0; i < mThreadNumber; ++i ){
			unsigned dummy;
			mThreads[ i ] = reinterpret_cast< HANDLE >( _beginthreadex( NULL, 0, execute, this, 0, &dummy ) );
			SetThreadPriority( mThreads[ i ], THREAD_PRIORITY_BELOW_NORMAL ); //優先度ちょっと下
		}
	}
	~ManagerImpl(){
		//終了待ち
		mLock.lock();
		mEndRequest = true;
		mLock.unlock();

		//スレッド終了待ち
		for ( int i = 0; i < mThreadNumber; ++i ){
			while ( true ){
				//終わらない間セマフォを増やしながらぐるぐる待つ。decreaseで待ってるのでincreaseしないと起きない。
				mQueueSemaphore.increase(); //キューに何か入っているように見せかけて起こす
				if ( WAIT_OBJECT_0 == WaitForSingleObject( mThreads[ i ], 1 ) ){ 
					CloseHandle( mThreads[ i ] );
					mThreads[ i ] = 0;
					break;
				}
			}
		}
		SAFE_DELETE_ARRAY( mThreads );
	}
	static unsigned WINAPI execute( void* p ){
		ManagerImpl* m = reinterpret_cast< ManagerImpl* >( p );
		m->execute();
		return 0;
	}
	void execute(){
		while ( true ){
			mQueueSemaphore.decrease(); //キュー追加を待つ

			mLock.lock();
			if ( mEndRequest ){
				mLock.unlock();
				break;
			}
			Entry e = mQueue.front();
			mQueue.pop_front(); //キューから取り出す
			mLock.unlock();

			( *( e.mThread ) )(); //実行
			e.mFinished->set(); //終了フラグ立てる
		}
	}
	void add( Thread* t, Event* finishFlag ){
		Entry e( t, finishFlag );

		mLock.lock();
		mQueue.push_back( e );
		mLock.unlock();

		mQueueSemaphore.increase(); //足しました
	}
	int getCoreNumber() const {
		return mCoreNumber;
	}
private:
	HANDLE* mThreads; //スレッドハンドル配列
	int mThreadNumber;
	bool mEndRequest;
	Semaphore mQueueSemaphore;
	Mutex mLock;
	list< Entry > mQueue; //要求待ち行列
	int mCoreNumber;
};
extern ManagerImpl* gManagerImpl;

} //namespace Threading
} //namespace GameLib
 
#endif
