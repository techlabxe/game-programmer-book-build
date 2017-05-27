#ifndef INCLUDED_GAMELIB_SHAREDPTR_H
#define INCLUDED_GAMELIB_SHAREDPTR_H

namespace GameLib{

/*参照カウンタ式共有ポインタ

スレッドセーフではないことに注意。
ConstSharedPtrは元の型のconst関数しか呼ばせたくない時のために
あるもので、ConstSharedPtrとあるところはSharedPtrでもいい。

ConstSharedPtr< int > foo(){
   return SharedPtr< int >( 0 );
}
や、
ConstSharedPtr< int > a = SharedPtr< int >();
がオーケーだということだ。
*/
template< class T > class SharedPtr{
public:
	SharedPtr();
	SharedPtr( T* p );
	SharedPtr( const SharedPtr< T >& p );
	~SharedPtr();
	SharedPtr& operator=( T* p );
	SharedPtr& operator=( const SharedPtr< T >& p );
	T& operator*();
	const T& operator*() const;
	T* operator->();
	const T* operator->() const;
	operator void*() const;
private:
	struct Reference{
		Reference( T* p );
		~Reference();
		void increment();
		int decrement();

		T* mPointer;
		unsigned mCount;
	};
	Reference* mReference;
};

//const版
template< class T > class ConstSharedPtr{
public:
	ConstSharedPtr();
	ConstSharedPtr( const SharedPtr< T >& );
	~ConstSharedPtr();
	ConstSharedPtr& operator=( const SharedPtr< T >& );
	const T& operator*() const;
	const T* operator->() const;
	operator void*() const;
private:
	SharedPtr< T > mPointer;
};

} //namespace GameLib
#include "GameLib/Base/Impl/SharedPtrImpl.h"

#endif