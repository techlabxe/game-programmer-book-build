#ifndef INCLUDED_GAMELIB_VECTORIMPL_H
#define INCLUDED_GAMELIB_VECTORIMPL_H

#include "GameLib/GameLib.h"
#include "GameLib/Base/Vector.h"

namespace GameLib{

template< class T > inline Vector< T >::Vector( int size, int capacity ) : 
mElements( 0 ),
mSize( size ),
mCapacity( capacity ){
	if ( capacity <= 0 ){
		mCapacity = mSize;
	}
	if ( mSize > 0 ){
		mElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mCapacity ) );
		for ( int i = 0; i < mSize; ++i ){
			new ( &mElements[ i ] ) T;
		}
	}
}

template< class T > inline Vector< T >::Vector( const Vector& a ) : 
mElements( 0 ),
mSize( a.mSize ),
mCapacity( a.mCapacity ){
	if ( mSize > 0 ){
		mElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mCapacity ) );
		for ( int i = 0; i < mSize; ++i ){
			new ( &mElements[ i ] ) T( a.mElements[ i ] ); //コピーコンストラクタ呼び出し
		}
	}
}

template< class T > inline Vector< T >::~Vector(){
	clear();
}

template< class T > inline void Vector< T >::operator=( const Vector& a ){ 
	//operator=()を書く時には自分を代入するケースに気をつけよう。
	if ( this == &a ){
		return;
	}
	//まず丸ごと削除。十分広い領域があっても、無駄になる可能性があるので削除する。
	clear();

	mSize = a.size;
	mCapacity = a.mCapacity;
	if ( mSize > 0 ){
		mElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mCapacity ) );
		for ( int i = 0; i < mSize; ++i ){
			new ( &mElements[ i ] ) T( a.mElements[ i ] ); //コピーコンストラクタ呼び出し
		}
	}
}


template< class T > inline void Vector< T >::push( const T& a ){
	if ( mSize == mCapacity ){ //場所がない。引越しを行う。
		//1.5倍+1サイズの領域を確保。+1は2未満でも増えるようにするため。
		mCapacity = ( mCapacity * 3 / 2 ) + 1;
		T* newElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mCapacity ) );
		//全コピーおよび古い要素の破棄
		for ( int i = 0; i < mSize; ++i ){
			new ( &newElements[ i ] ) T( mElements[ i ] );
			mElements[ i ].~T();
		}
		//旧領域の破棄
		OPERATOR_DELETE( mElements );
		//差し替え
		mElements = newElements;
	}
	//新要素の追加
	new ( &mElements[ mSize ] ) T( a );
	//サイズ更新
	++mSize;
}

template< class T > inline T* Vector< T >::push(){
	if ( mSize == mCapacity ){ //場所がない。引越しを行う。
		//1.5倍+1サイズの領域を確保。+1は2未満でも増えるようにするため。
		mCapacity = ( mCapacity * 3 / 2 ) + 1;
		T* newElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mCapacity ) );
		//全コピーおよび古い要素の破棄
		for ( int i = 0; i < mSize; ++i ){
			new ( &newElements[ i ] ) T( mElements[ i ] );
			mElements[ i ].~T();
		}
		//旧領域の破棄
		OPERATOR_DELETE( mElements );
		//差し替え
		mElements = newElements;
		newElements = 0;
	}
	//新要素の追加
	new ( &mElements[ mSize ] ) T;
	//サイズ更新
	++mSize;
	//新要素のポインタを返す
	return &mElements[ mSize - 1 ];
}

template< class T > inline void Vector< T >::clear(){
	//デストラクタ呼び出し
	for ( int i = 0; i < mSize; ++i ){
		mElements[ i ].~T();
	}
	//領域開放
	OPERATOR_DELETE( mElements );
	mSize = mCapacity = 0;
}

template< class T > inline int Vector< T >::size() const {
	return mSize;
}

template< class T > inline void Vector< T >::setSize( int n ){
	//指定サイズが今のcapacity以下なら引越ししない
	if ( n < mCapacity ){
		for ( int i = n; i < mSize; ++i ){ //ただし後ろの方の解放は必要。5個あって3にするなら添え字3,4は解放する必要がある
			mElements[ i ].~T();
		}
	}else if ( n > mCapacity ){ //ダメだ。引越し
		mCapacity = n;
		T* newElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mCapacity ) );
		//全コピーおよび古い要素の破棄
		for ( int i = 0; i < mSize; ++i ){
			new ( &newElements[ i ] ) T( mElements[ i ] );
			mElements[ i ].~T();
		}
		//旧領域の破棄
		OPERATOR_DELETE( mElements );
		//差し替え
		mElements = newElements;
		newElements = 0;
		//何もない領域にデフォルトコンストラクタ呼び出し
		for ( int i = mSize; i < n; ++i ){
			new ( &mElements[ i ] ) T;
		}
	}
	mSize = n;
}

template< class T > inline T& Vector< T >::operator[]( int i ){
	ASSERT( i >= 0 && i < mSize && "Range Error!" );
	return mElements[ i ];
}

template< class T > inline const T& Vector< T >::operator[]( int i ) const {
	ASSERT( i >= 0 && i < mSize && "Range Error!" );
	return mElements[ i ];
}

template< class T > inline void Vector< T >::moveTo( Vector* a ){
	a->clear();
	a->mElelents = mElements;
	a->mSize = mSize;
	a->mCapacity = mCapacity;
	mElements = 0;
	mSize = mCapacity = 0;
}

} //namespace GameLib

#endif
