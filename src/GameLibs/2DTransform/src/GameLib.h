#ifndef INCLUDED_GAMELIB_GAMELIB_H
#define INCLUDED_GAMELIB_GAMELIB_H

namespace GameLib{
	void halt( const char* filename, int line, const char* message );
}

//マクロ類
#define STRONG_ASSERT( exp ) ( ( !! ( exp ) ) || ( GameLib::halt( __FILE__, __LINE__, #exp ), 0 ) ) //この行の意味を理解するのは相当難しい
#define HALT( exp ) { GameLib::halt( __FILE__, __LINE__, #exp ); }
#define SAFE_DELETE( x ) { delete ( x ); ( x ) = 0; }
#define SAFE_DELETE_ARRAY( x ) { delete[] ( x ); ( x ) = 0; }

//デバグとリリースで分岐するもの
#ifndef NDEBUG
#define ASSERT( exp ) ( ( !! ( exp ) ) || ( GameLib::halt( __FILE__, __LINE__, #exp ), 0 ) )
#else //NDEBUG
#define ASSERT( exp )
#endif //NDEBUG

//ベースサービスヘッダ
#include "Base/DebugStream.h"

#endif
