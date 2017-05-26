#include "GameLib/Threading/Functions.h"
using namespace GameLib;
#include "CriticalSection.h"

CriticalSection::CriticalSection() : mEntered( 0 ){
}

void CriticalSection::enter(){
	while ( Threading::compareAndSwap( &mEntered, 1, 0 ) != 0 ){
		;
	}
}

void CriticalSection::leave(){
	Threading::compareAndSwap( &mEntered, 0, mEntered );
}

