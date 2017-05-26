#include <windows.h>
#undef min
#undef max
#include "GameLib/GameLib.h"
#include "GameLib/Threading/Semaphore.h"
#include "GameLib/Base/Impl/ReferenceType.h"

namespace GameLib{
namespace Threading{

class Semaphore::Impl : public ReferenceType{
public:
	Impl( int startCount, int maxCount ) : mMaxCount( maxCount ){
		mHandle = CreateSemaphore( NULL, startCount, mMaxCount, NULL );
	}
	~Impl(){
		CloseHandle( mHandle );
	}
	void decrease(){
		WaitForSingleObject( mHandle, INFINITE );
	}
	void increase( int value ){
		LONG prev;
		ReleaseSemaphore( mHandle, value, &prev );
		STRONG_ASSERT( prev + value <= mMaxCount );
	}
	HANDLE mHandle;
	int mMaxCount;
};

Semaphore Semaphore::create( int startCount, int maxCount ){
	STRONG_ASSERT( startCount >= 0 );
	if ( maxCount < startCount ){
		maxCount = startCount;
	}
	Semaphore r;
	r.mImpl = NEW Impl( startCount, maxCount );
	return r;
}

void Semaphore::decrease(){
	mImpl->decrease();
}

void Semaphore::increase( int v ){
	STRONG_ASSERT( v > 0 );
	mImpl->increase( v );
}

#define TYPE Semaphore
#include "GameLib/Base/Impl/ReferenceTypeTemplate.h"


} //namespace Threading
} //namespace GameLib
