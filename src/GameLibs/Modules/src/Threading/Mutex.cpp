#include <windows.h>
#undef min
#undef max
#include "GameLib/GameLib.h"
#include "GameLib/Threading/Mutex.h"
#include "GameLib/Base/Impl/ReferenceType.h"

namespace GameLib{
namespace Threading{

//多重アンロック、多重ロック等々を検出するために、中身はセマフォにしておく。
//Release版でCriticalSectionに切り替えるのもいいかもしれない。
class Mutex::Impl : public ReferenceType{
public:
	Impl() : mHandle( 0 ){
		mHandle = CreateSemaphore( NULL, 1, 1, NULL );
	}
	~Impl(){
		CloseHandle( mHandle );
	}
	void lock(){
		WaitForSingleObject( mHandle, INFINITE );
	}
	void unlock(){
		LONG prev;
		ReleaseSemaphore( mHandle, 1, &prev );
		STRONG_ASSERT( prev == 0 && "unlock() is called twice! Check lock-unlock pair." );
	}
	HANDLE mHandle;
};

Mutex Mutex::create(){
	Mutex r;
	r.mImpl = NEW Impl();
	return r;
}

void Mutex::lock(){
	mImpl->lock();
}

void Mutex::unlock(){
	mImpl->unlock();
}

#define TYPE Mutex
#include "GameLib/Base/Impl/ReferenceTypeTemplate.h"


} //namespace Threading
} //namespace GameLib
