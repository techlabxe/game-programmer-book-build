#ifndef INCLUDED_GAMELIB_TANKIMPL_H
#define INCLUDED_GAMELIB_TANKIMPL_H

#include "GameLib/GameLib.h"
#include "GameLib/Base/Tank.h"

namespace GameLib{

template< class T > inline Tank< T >::Tank( int blockSize ) :
mBlockSize( blockSize ),
mSize( 0 ),
mLastBlock( &mHead ),
mLastPos( blockSize ),
mCurrentBlock( &mHead ),
mCurrentPos( blockSize ){
	mHead.mNext = &mHead;
	mHead.mElements = 0;
}

template< class T > inline Tank< T >::~Tank(){
	clear();
}

template< class T > inline void Tank< T >::setBlockSize( int blockSize ){
	ASSERT( ( mSize == 0 ) && "NOT EMPTY!. you must call clear()." );
	if ( mSize > 0 ){
		clear();
	}
	mBlockSize = blockSize;
	mLastPos = blockSize;
}

template< class T > inline int Tank< T >::size() const {
	return mSize;
}

template< class T > inline void Tank< T >::clear(){
	Block* n = mHead.mNext;
	while ( n != &mHead ){
		Block* next = n->mNext;
		int destructN = ( mLastBlock == n ) ? mLastPos : mBlockSize;
		for ( int i = 0; i < destructN; ++i ){
			n->mElements[ i ].~T();
		}
		OPERATOR_DELETE( n->mElements );
		n->mNext = 0;
		SAFE_DELETE( n );
		n = next;
	}
	mSize = 0;
	mHead.mNext = mLastBlock = mCurrentBlock = &mHead;
	mLastPos = mCurrentPos = mBlockSize;
}

template< class T > inline void Tank< T >::add( const T& v ){
	//バケツなければ作る
	if ( mLastPos == mBlockSize ){
		mLastBlock->mNext = new Block;
		if ( mLastBlock == &mHead ){//最初のノードにイテレータセット
			mCurrentBlock = mLastBlock->mNext;
			mCurrentPos = 0;
		}
		mLastBlock = mLastBlock->mNext;
		mLastPos = 0;
		mLastBlock->mNext = &mHead;
		mLastBlock->mElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mBlockSize ) );
	}
	new ( &mLastBlock->mElements[ mLastPos ] ) T( v );
	++mLastPos;
	++mSize;
}

template< class T > inline T* Tank< T >::add(){
	//バケツなければ作る
	if ( mLastPos == mBlockSize ){
		mLastBlock->mNext = new Block;
		if ( mLastBlock == &mHead ){//最初のノードにイテレータセット
			mCurrentBlock = mLastBlock->mNext;
			mCurrentPos = 0;
		}
		mLastBlock = mLastBlock->mNext;
		mLastPos = 0;
		mLastBlock->mNext = &mHead;
		mLastBlock->mElements = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mBlockSize ) );
	}
	T* r = &mLastBlock->mElements[ mLastPos ];
	new ( r ) T;
	++mLastPos;
	++mSize;
	return r;
}

template< class T > inline void Tank< T >::toNext(){
	if ( mCurrentBlock == mLastBlock ){
		++mCurrentPos;
		if ( mCurrentPos == mLastPos ){
			mCurrentBlock = &mHead; //おしまい
		}
	}else{
		++mCurrentPos;
		if ( mCurrentPos == mBlockSize ){
			mCurrentBlock = mCurrentBlock->mNext;
			mCurrentPos = 0;
		}
	}
}

template< class T> inline bool Tank< T >::isEnd() const {
	return ( mCurrentBlock == &mHead );
}

template< class T> inline void Tank< T >::rewind(){
	mCurrentBlock = mHead.mNext;
	mCurrentPos = 0;
}

template< class T > inline const T* Tank< T >::get() const {
	ASSERT( ( mCurrentBlock != mLastBlock ) || ( mCurrentPos < mLastPos ) && "exceeded the range." );
	return &mCurrentBlock->mElements[ mCurrentPos ];
}

template< class T > inline T* Tank< T >::get(){
	ASSERT( ( mCurrentBlock != mLastBlock ) || ( mCurrentPos < mLastPos ) && "exceeded the range." );
	return &mCurrentBlock->mElements[ mCurrentPos ];
}

template< class T > inline void Tank< T >::copyTo( T* o ) const {
	int pos = 0;
	Block* n = mHead.mNext;
	while ( n != &mHead ){
		Block* next = n->mNext;
		int elementN = ( mLastBlock == n ) ? mLastPos : mBlockSize;
		for ( int i = 0; i < elementN; ++i ){
			o[ pos ] = n->mElements[ i ];
			++pos;
		}
		n = next;
	}
}

template< class T > inline void Tank< T >::copyTo( Array< T >* o ) const {
	o->setSize( mSize );
	if ( mSize > 0 ){
		copyTo( &( ( *o )[ 0 ] ) );
	}
}

} //namespace GameLib

#endif
