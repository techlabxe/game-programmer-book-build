#ifndef INCLUDED_GAMELIB_POOLIMPL_H
#define INCLUDED_GAMELIB_POOLIMPL_H

#include "GameLib/GameLib.h"
#include "GameLib/Base/Pool.h"

namespace GameLib{

template< class T > inline Pool< T >::Pool( int defaultBlockSize ) :
mDefaultBlockSize( defaultBlockSize ),
mLastBlock( &mHead ){
	mHead.mNext = 0;
	mHead.mElements = 0;
	mHead.mUsedNumber = 0;
	mHead.mSize = 0;
}

template< class T > inline Pool< T >::~Pool(){
	clear();
}

template< class T > inline void Pool< T >::setDefaultBlockSize( int defaultBlockSize ){
	mDefaultBlockSize = defaultBlockSize;
}

template< class T > inline void Pool< T >::clear(){
	Block* b = mHead.mNext;
	while ( b ){
		Block* next = b->mNext;
		for ( int i = 0; i < b->mUsedNumber; ++i ){
			b->mElements[ i ].~T();
		}
		OPERATOR_DELETE( b->mElements );
		b->mNext = 0;
		SAFE_DELETE( b );
		b = next;
	}
	mHead.mNext = 0;
	mLastBlock = &mHead;
}

template< class T > inline T* Pool< T >::allocate( int n ){
	T* r = 0;
	Block* b = mLastBlock;
	if ( b->mSize - b->mUsedNumber < n ){ //足りない場合確保。
		//標準ブロックサイズで足りないなら必要数分だけアロケート
		int allocateSize = ( n > mDefaultBlockSize ) ? n : mDefaultBlockSize;
		b = NEW Block;
		mLastBlock->mNext = b;
		b->mNext = 0;
		b->mElements = static_cast< T* >( OPERATOR_NEW( sizeof( T )  * allocateSize ) );
		b->mSize = allocateSize;
		b->mUsedNumber = 0;
		mLastBlock = b;
	}
	//新要素構築
	r = &b->mElements[ b->mUsedNumber ];
	for ( int i = 0; i < n; ++i ){
		new ( &b->mElements[ b->mUsedNumber + i ] ) T;
	}
	b->mUsedNumber += n;
	return r;
}

template< class T > inline T* Pool< T >::allocate( const T* a, int n ){
	T* r = 0;
	Block* b = mLastBlock;
	if ( b->mSize - b->mUsedNumber < n ){ //足りない場合確保。
		//標準ブロックサイズで足りないなら必要数分だけアロケート
		int allocateSize = ( n > mDefaultBlockSize ) ? n : mDefaultBlockSize;
		b = NEW Block;
		mLastBlock->mNext = b;
		b->mNext = 0;
		b->mElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * allocateSize ) );
		b->mSize = allocateSize;
		b->mUsedNumber = 0;
		mLastBlock = b;
	}
	//新要素構築
	r = &b->mElements[ b->mUsedNumber ];
	for ( int i = 0; i < n; ++i ){
		new ( &b->mElements[ b->mUsedNumber + i ] ) T( a[ i ] );
	}
	b->mUsedNumber += n;
	return r;
}

} //namespace GameLib

#endif
