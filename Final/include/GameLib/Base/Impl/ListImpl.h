#ifndef INCLUDED_GAMELIB_LISTIMPL_H
#define INCLUDED_GAMELIB_LISTIMPL_H

#include "GameLib/GameLib.h"
#include "GameLib/Base/List.h"

/*
<注意>
添え字でmCapacityが使われていたら、それはダミーノードである。
mNodes[ mCapacity ].mNextは先頭、
mNodes[ mCapacity ].mPrevは末尾になっている。
*/

namespace GameLib{

//先頭と末尾が特別扱いされないようにちょっと工夫をしている。
template< class T > inline List< T >::List() : 
mValues( 0 ),
mNodes( 0 ),
mCapacity( 0 ),
mSize( 0 ){
}

//先頭と末尾が特別扱いされないようにちょっと工夫をしている。
template< class T > inline List< T >::List( int capacity ) : 
mValues( 0 ),
mNodes( 0 ),
mCapacity( 0 ),
mSize( 0 ){
	setCapacity( capacity );
}

template< class T > inline List< T >::~List(){
	if ( mNodes ){
		//単体デストラクト
		int pos = mNodes[ mCapacity ].mNext;
		while ( pos < mCapacity ){ //ダミーでなければ
			mValues[ pos ].~T(); //デストラクト
			pos = mNodes[ pos ].mNext;
		}
		OPERATOR_DELETE( mValues ); //生delete
		SAFE_DELETE_ARRAY( mNodes );
	}
}

template< class T > inline void List< T >::setCapacity( int capacity ){
	ASSERT( mSize == 0 && "NOT EMPTY! call clear()." );
	//まずクリア
	if ( mCapacity > 0 ){
		mEmptyStack.clear();
		clear();
		OPERATOR_DELETE( mValues ); //生delete
		SAFE_DELETE_ARRAY( mNodes );
	}
	//再確保
	mCapacity = capacity;
	if ( capacity <= 0 ){ //0サイズ無視
		return;
	}
	mNodes = NEW Node[ mCapacity + 1 ];
	//ノード配列確保。コンストラクタが不要なのでmalloc直
	mValues = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mCapacity ) );
	//空き番号スタック
	mEmptyStack.setCapacity( mCapacity );
	for ( int i = 0; i < mCapacity; ++i ){
		mEmptyStack.push( i ); //空き番号リストに詰めていく
	}
	//mCapacity番は特別。最初の要素の前かつ最後の要素の後にあるダミー要素
	mNodes[ mCapacity ].mNext = mNodes[ mCapacity ].mPrev = mCapacity;
}

template< class T > inline int List< T >::capacity() const {
	return mCapacity;
}

template< class T > inline int List< T >::addAfter( int position, const T& v ){
	ASSERT( position >= 0 && position <= mCapacity ); //イコールはありうる
	//空き場所を取得
	int newPos;
	mEmptyStack.pop( &newPos );
	//足す場所はaddPosition
	Node* e = &mNodes[ newPos ];
	//指定の奴を取り出す
	Node* cur = &mNodes[ position ];
	//次の奴を取り出す
	Node* next = &mNodes[ cur->mNext ];
	//値格納(コピーコンストラクタ呼び出し)
	new ( &mValues[ newPos ] ) T( v );
	e->mPrev = position; //指定場所が前に
	e->mNext = cur->mNext; //指定の次のが次に
	//指定の奴は前になるので、次は足した奴になる
	cur->mNext = newPos;
	//指定の次の奴は後になるので、前は足した奴になる
	next->mPrev = newPos;
	++mSize;

	return newPos;
}

template< class T > inline int List< T >::addBefore( int position, const T& v ){
	ASSERT( position >= 0 && position <= mCapacity ); //イコールはありうる
	//空き場所を取得
	int newPos;
	mEmptyStack.pop( &newPos );
	//足す場所はaddPosition
	Node* e = &mNodes[ newPos ];
	//指定の奴を取り出す
	Node* cur = &mNodes[ position ];
	//前の奴を取り出す
	Node* prev = &mNodes[ cur->mPrev ];
	//値格納(コンストラクタ呼び出し)
	new ( &mValues[ newPos ] ) T( v );
	e->mPrev = cur->mPrev; //指定の前の奴が前
	e->mNext = position; //指定の奴が次
	//指定の奴は後になるので、前は足した奴になる
	cur->mPrev = newPos;
	//指定の前の奴は前になるので、後は足した奴になる
	prev->mNext = newPos;
	++mSize;

	return newPos;
}

template< class T > inline int List< T >::addHead( const T& v ){
	return addAfter( mCapacity, v ); //ダミー要素のおかげでこう書ける。
}

template< class T > inline int List< T >::addTail( const T& v ){
	return addBefore( mCapacity, v ); //ダミー要素のおかげでこう書ける
}

template< class T > inline void List< T >::remove( int position ){
	ASSERT( mSize > 0 );
	//指定の奴
	Node* cur = &mNodes[ position ];
	//次
	Node* next = &mNodes[ cur->mNext ];
	//前
	Node* prev = &mNodes[ cur->mPrev ];
	//前の奴の次を、次にする。
	prev->mNext = cur->mNext;
	//次の奴の前を、前にする。
	next->mPrev = cur->mPrev;
	//デストラクタを明示的に呼ぶ
	mValues[ position ].~T();
	//サイズ減少
	--mSize;
	//空きスタックにプッシュ
	mEmptyStack.push( position );
}

template< class T > inline void List< T >::removeHead(){ //これはほとんど別名。
	int pos = mNodes[ mCapacity ].mNext;
	if ( pos < mCapacity ){ //ダミーは消せないからな
		remove( pos  );
	}
}

template< class T > inline void List< T >::removeTail(){ //これはほとんど別名。
	int pos = mNodes[ mCapacity ].mPrev;
	if ( pos < mCapacity ){ //ダミーは消せないからな
		remove( pos  );
	}
}

template< class T> inline void List< T >::clear(){
	int pos = mNodes[ mCapacity ].mNext;
	while ( pos < mCapacity ){ //ダミーでなければ
		mValues[ pos ].~T(); //デストラクト
		pos = mNodes[ pos ].mNext;
	}
	mNodes[ mCapacity ].mNext = mCapacity;
	mNodes[ mCapacity ].mPrev = mCapacity;
	mSize = 0;
	//空スタック詰めなおし
	mEmptyStack.clear();
	for ( int i = 0; i < mCapacity; ++i ){
		mEmptyStack.push( i ); //空き番号リストに詰めていく
	}
}

template< class T > inline const T* List< T >::value( int position ) const {
	ASSERT( position >= 0 && position < mCapacity );
	return &mValues[ position ];
}

template< class T > inline T* List< T >::value( int position ){
	ASSERT( position >= 0 && position < mCapacity );
	return &mValues[ position ];
}

template< class T > inline int List< T >::next( int position ) const {
	ASSERT( position >= 0 && position <= mCapacity ); //ダミーも来得るので<=
	return mNodes[ position ].mNext;
}

template< class T > inline int List< T >::previous( int position ) const {
	ASSERT( position >= 0 && position <= mCapacity );//ダミーも来得るので<=
	return mNodes[ position ].mPrev;
}

template< class T > inline int List< T >::first() const {
	return next( mCapacity );
}

template< class T > inline int List< T >::last() const {
	return getPrev( mCapacity );
}

template< class T > inline bool List< T >::isEnd( int position ) const {
	return ( position == mCapacity );
}

template< class T > inline void List< T >::copyTo( T* a ) const {
	int p = mNodes[ mCapacity ].mNext;
	int idx = 0;
	while ( p != mCapacity ){
		a[ idx ] = mValues[ p ];;
		++idx;
		p = mNodes[ p ].mNext;
	}
}

template< class T > inline void List< T >::copyTo( Array< T >* a ) const {
	a->setSize( mSize );
	if ( mSize > 0 ){
		copyTo( &( ( *a )[ 0 ] ) );
	}
}
		




} //namespace GameLib

#endif
