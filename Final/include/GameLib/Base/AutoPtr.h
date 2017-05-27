#ifndef INCLUDED_GAMELIB_AUTOPTR_H
#define INCLUDED_GAMELIB_AUTOPTR_H

namespace GameLib{

template< class T > class AutoPtr{
public:
	AutoPtr( T* p = 0 );
	~AutoPtr();
	//代入:中身があれば即削除(これを利用して、=0で好きなタイミングで消せる)
	AutoPtr& operator=( T* p );
	T& operator*();
	const T& operator*() const;
	T* operator->();
	const T* operator->() const;
	operator void*() const; //これのおかげでif ( p ){ と書ける。
private:
	void operator=( AutoPtr< T >& ); //代入は禁止。管理の移譲はわかりにくい。
	AutoPtr( AutoPtr< T >& ); //コピーコンストラクタも禁止。管理の移譲はわかりにくい。

	T* mPointer;
};

} //namespace GameLib
#include "GameLib/Base/Impl/AutoPtrImpl.h"

#endif