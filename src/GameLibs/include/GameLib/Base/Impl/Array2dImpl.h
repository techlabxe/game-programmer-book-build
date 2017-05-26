#ifndef INCLUDED_GAMELIB_ARRAY2DIMPL_H
#define INCLUDED_GAMELIB_ARRAY2DIMPL_H

#include "GameLib/GameLib.h"
#include "GameLib/Base/Array2d.h"

namespace GameLib{

template< class T > inline Array2d< T >::Array2d() :
mElements( 0 ){
	mSize[ 0 ] = mSize[ 1 ] = 0;
}

template< class T > inline Array2d< T >::Array2d( int size0, int size1 ) :
mElements( 0 ){
	mSize[ 0 ] = size0;
	mSize[ 1 ] = size1;
	int size = size0 * size1;
	if ( size > 0 ){
		mElements = NEW T[ size ];
	}
}

template< class T > inline Array2d< T >::~Array2d(){
	clear();
}

template< class T > inline void Array2d< T >::setSize( int size0, int size1 ){
	ASSERT( !mElements && "NOT EMPTY!. you must call clear()." );
	mSize[ 0 ] = size0;
	mSize[ 1 ] = size1;
	mElements = NEW T[ size0 * size1 ];
}

template< class T > inline int Array2d< T >::size( int i ) const{
	ASSERT( i >= 0 && i < 2 );
	return mSize[ i ];
}

template< class T > inline void Array2d< T >::clear(){
	SAFE_DELETE_ARRAY( mElements );
	mSize[ 0 ] = mSize[ 1 ] = 0;
}

template< class T > inline const T& Array2d< T >::operator()( int i, int j ) const {
	ASSERT( i >= 0 && i < mSize[ 0 ] && "i range error." );
	ASSERT( j >= 0 && j < mSize[ 1 ] && "j range error." );
	return mElements[ j * mSize[ 0 ] + i ];
}

template< class T > inline T& Array2d< T >::operator()( int i, int j ){
	ASSERT( i >= 0 && i < mSize[ 0 ] && "i range error." );
	ASSERT( j >= 0 && j < mSize[ 1 ] && "j range error." );
	return mElements[ j * mSize[ 0 ] + i ];
}

} //namespace GameLib

#endif