#ifndef INCLUDED_GAMELIB_HASH_H
#define INCLUDED_GAMELIB_HASH_H

namespace GameLib{

//汎用ハッシュ計算クラス
template< class T > class Hash{
public:
	unsigned value( const T&, unsigned mod ) const;
	bool isEqual( const T&, const T& ) const;
};

} //namespace GameLib
#include "GameLib/Base/Impl/HashImpl.h" //中身はこの中

#endif

