#ifndef INCLUDED_GAMELIB_QUEUEIMPL_H
#define INCLUDED_GAMELIB_QUEUEIMPL_H

#include "GameLib/GameLib.h"
#include "GameLib/Base/Queue.h"

namespace GameLib{

template< class T > inline Queue< T >::Queue() :
mElements( 0 ),
mSize( 0 ),
mCapacity( 0 ),
mFront( 0 ){
}

template< class T > inline Queue< T >::Queue( int capacity ) :
mElements( 0 ),
mSize( 0 ),
mCapacity( 0 ),
mFront( 0 ){
	if ( capacity <= 0 ){
		return;
	}
	mCapacity = capacity;
	//コンストラクタを呼びたくないので生new
	mElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mCapacity ) );
}

template< class T > inline Queue< T >::~Queue(){
	if ( mElements ){
		clear();
		OPERATOR_DELETE( mElements );
	}
}

template< class T > inline void Queue< T >::setCapacity( int capacity ){
	ASSERT( mSize == 0 && "NOT EMPTY!. you must call clear()." );
	//一旦解放
	if ( mElements ){
		clear();
		OPERATOR_DELETE( mElements );
	}
	mCapacity = mFront = 0;
	if ( capacity <= 0 ){
		return;
	}
	mCapacity = capacity;
	//再確保
	//コンストラクタを呼びたくないので生new
	mElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mCapacity ) );
}

template< class T > inline int Queue< T >::capacity() const {
	return mCapacity;
}

template< class T > inline int Queue< T >::size() const {
	return mSize;
}

template< class T > inline void Queue< T >::clear(){
	int back = mFront + mSize - 1;
	if ( back < mCapacity ){
		for ( int i = mFront; i <= back; ++i ){
			mElements[ i ].~T(); //明示的にデストラクト
		}
	}else{
		//front->last
		for ( int i = mFront; i < mCapacity; ++i ){
			mElements[ i ].~T(); //明示的にデストラクト
		}
		//0->back
		for ( int i = 0; i <= back; ++i ){
			mElements[ i ].~T();
		}
	}
	mFront = mSize = 0;
}

template< class T > inline void Queue< T >::pushBack( const T& v ){
	ASSERT( mSize < mCapacity && "Full! can't push!" );
	int back = mFront + mSize;
	if ( back >= mCapacity ){
		back -= mCapacity;
	}
	new ( &mElements[ back ] ) T( v );
	++mSize;
}

template< class T > inline T* Queue< T >::pushBack(){
	ASSERT( mSize < mCapacity && "Full! can't push!" );
	int back = mFront + mSize;
	if ( back >= mCapacity ){
		back -= mCapacity;
	}
	new ( &mElements[ back ] ) T;
	++mSize;
	return &mElements[ back ];
}


template< class T > inline void Queue< T >::popFront(){
	ASSERT( mSize > 0 );
	mElements[ mFront ].~T(); //明示的デストラクト
	++mFront;
	if ( mFront == mCapacity ){
		mFront = 0;
	}
	--mSize;
}

template< class T > inline void Queue< T >::popFront( T* o ){
	ASSERT( mSize > 0 );
	o = mElements[ mFront ];
	mElements[ mFront ].~T(); //明示的デストラクト
	++mFront;
	if ( mFront == mCapacity ){
		mFront = 0;
	}
	--mSize;
}

template< class T > inline const T* Queue< T >::front() const {
	ASSERT( mSize > 0 );
	return &mElements[ mFront ];
}

template< class T > inline T* Queue< T >::front(){
	ASSERT( mSize > 0 );
	return &mElements[ mFront ];
}

} //namespace GameLib

#endif