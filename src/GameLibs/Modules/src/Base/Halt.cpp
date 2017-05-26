#include "GameLib/GameLib.h"
#include <windows.h>
#include <cstdio>

bool gThrowWhenHalt = false;

//このファイルではnewしないよう配慮する。
//newの中からも呼ばれうるからだ。
namespace GameLib{

void halt( const char* filename, int line, const char* message ){
	char buf[ 8192 ];
	sprintf_s( buf, 8192, "%s:%d : %s", filename, line, message );
	if ( gThrowWhenHalt ){
		::OutputDebugStringA( buf );
		::OutputDebugStringA( "\n" );
		throw EXCEPTION_IGNORE; //基本は無視
	}else{
		//メッセージボックスを出して
		MessageBoxA( 0, buf, "Critical Error!", MB_OK | MB_ICONERROR );
		//即死
		*reinterpret_cast< int* >( 0 ) = 0;
	}
}

} //namespace GameLib

