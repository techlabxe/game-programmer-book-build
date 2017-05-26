#ifndef INCLUDED_GAMELIB_HASH_MULTIMAPIMPL_H
#define INCLUDED_GAMELIB_HASH_MULTIMAPIMPL_H

#include "GameLib/GameLib.h"
#include "GameLib/Base/HashMultiMap.h"
#include "GameLib/Base/PrimeNumber.h"

/*
<注意>
添え字でmCapacity + i (0<=i<mTableSize)が使われていたら、それはダミーノードである。
mNodes[ mCapacity + i ].mNextはmNodes[ mCapacity + i + 1 ]をさしている。
*/

namespace GameLib{

//先頭と末尾が特別扱いされないようにちょっと工夫をしている。
template< class K, class V, class H > inline HashMultiMap< K, V, H >::HashMultiMap() : 
mNexts( 0 ),
mKeys( 0 ),
mValues( 0 ),
mCapacity( 0 ),
mSize( 0 ),
mTableSize( 0 ){
	ASSERT( sizeof( K ) != 0 ); //ありえないよね？
}

//先頭と末尾が特別扱いされないようにちょっと工夫をしている。
template< class K, class V, class H > inline HashMultiMap< K, V, H >::HashMultiMap( int capacity, int tableSize ) : 
mNexts( 0 ),
mKeys( 0 ),
mValues( 0 ),
mCapacity( 0 ),
mSize( 0 ),
mTableSize( 0 ){
	ASSERT( sizeof( K ) != 0 ); //ありえないよね？
	setCapacity( capacity, tableSize );
}

template< class K, class V, class H > inline HashMultiMap< K, V, H >::~HashMultiMap(){
	if ( mNexts ){
		int pos = mNexts[ mCapacity ];
		while ( pos < mCapacity + mTableSize ){ //ノードがあれば
			if ( pos < mCapacity ){
				mKeys[ pos ].~K();
				mValues[ pos ].~V();
			}
			pos = mNexts[ pos ];
		}
		OPERATOR_DELETE( mKeys ); //生delete
		OPERATOR_DELETE( mValues ); //生delete
		SAFE_DELETE_ARRAY( mNexts );
	}
}

template< class K, class V, class H > inline void HashMultiMap< K, V, H >::setCapacity( int capacity, int tableSize ){
	ASSERT( mSize == 0 && "NOT EMPTY! call clear()." );
	//まずクリア
	if ( mCapacity > 0 ){
		mEmptyStack.clear();
		clear();
		OPERATOR_DELETE( mValues ); //生delete
		OPERATOR_DELETE( mKeys ); //生delete
		SAFE_DELETE_ARRAY( mNexts );
	}
	//再確保
	mCapacity = capacity;
	if ( capacity <= 0 ){ //0サイズ無視
		return;
	}	
	if ( tableSize == 0 ){ //自動でcapacity以上の素数を設定
		tableSize = capacity;
	}
	tableSize = PrimeNumber::next( tableSize );
	if ( tableSize < 3 ){
		tableSize = 3; //最低で3
	}
	mTableSize = tableSize;
	//次配列 テーブルサイズ分だけダミーを足すことに注意
	mNexts = NEW int[ mCapacity + mTableSize ];
	//値配列確保。コンストラクタが不要なので生new
	mKeys = static_cast< K* >( OPERATOR_NEW( sizeof( K ) * mCapacity ) );
	mValues = static_cast< V* >( OPERATOR_NEW( sizeof( V ) * mCapacity ) );
	//空き番号スタック
	mEmptyStack.setCapacity( mCapacity );
	for ( int i = 0; i < mCapacity; ++i ){
		mEmptyStack.push( i ); //空き番号リストに詰めていく
	}
	//mCapacity+0からmCapacity+mTableSize-1番はダミー要素。各列の先頭の前に位置する。
	for ( int i = 0; i < mTableSize; ++i ){
		mNexts[ mCapacity + i ] = mCapacity + i + 1;
	}
}

template< class K, class V, class H > inline int HashMultiMap< K, V, H >::capacity() const {
	return mCapacity;
}

template< class K, class V, class H > inline int HashMultiMap< K, V, H >::add( const K& k, const V& v ){
	ASSERT( mSize < mCapacity );
	//ハッシュ関数算出
	int h = H().value( k, mTableSize );
	//検索
	int p = mNexts[ mCapacity + h ];
	while ( p < mCapacity ){
		if ( H().isEqual( mKeys[ p ], k ) ){
			break;
		}
		p = mNexts[ p ];
	}
	int newPos;
	mEmptyStack.pop( &newPos );
	new ( &mKeys[ newPos ] ) K( k ); //コピーコンストラクタ呼び出し
	new ( &mValues[ newPos ] ) V( v ); //コピーコンストラクタ呼び出し
	if ( p >= mCapacity ){ //ない時は先頭に足す
		//テーブル先頭に挿入
		int head = mCapacity + h;
		mNexts[ newPos ] = mNexts[ head ]; //頭ダミーの次をセット
		mNexts[ head ] = newPos; //前のリンクを新ノードにつなぐ
	}else{ //ある時は直後に足す
		mNexts[ newPos ] = mNexts[ p ];
		mNexts[ p ] = newPos; //前のリンクを新ノードにつなぐ
	}
	++mSize;
	return newPos;
}

template< class K, class V, class H > inline int HashMultiMap< K, V, H >::add( const K& k ){
	ASSERT( mSize < mCapacity );
	//ハッシュ関数算出
	int h = H().value( k, mTableSize );
	//検索
	int p = mNexts[ mCapacity + h ];
	while ( p < mCapacity ){
		if ( H().isEqual( mKeys[ p ], k ) ){
			break;
		}
		p = mNexts[ p ];
	}
	int newPos;
	mEmptyStack.pop( &newPos );
	new ( &mKeys[ newPos ] ) K( k ); //コピーコンストラクタ呼び出し
	new ( &mValues[ newPos ] ) V;
	if ( p >= mCapacity ){ //ない時は先頭に足す
		//テーブル先頭に挿入
		int head = mCapacity + h;
		mNexts[ newPos ] = mNexts[ head ]; //頭ダミーの次をセット
		mNexts[ head ] = newPos; //前のリンクを新ノードにつなぐ
	}else{ //ある時は直後に足す
		mNexts[ newPos ] = mNexts[ p ];
		mNexts[ p ] = newPos; //前のリンクを新ノードにつなぐ
	}
	++mSize;
	return newPos;
}

template< class K, class V, class H > inline int HashMultiMap< K, V, H >::find( const K& k ) const{
	int h = H().value( k, mTableSize );

	int p = mNexts[ mCapacity + h ];
	while ( p < mCapacity ){
		if ( H().isEqual( mKeys[ p ], k ) ){
			return p;
		}
		p = mNexts[ p ];
	}
	return mCapacity + mTableSize;
}

template< class K, class V, class H > inline bool HashMultiMap< K, V, H >::remove( int position ){
	//pの場所にある要素のハッシュ値をハッシュ値を計算しないで知りたい。
	//そこで、一旦ヘッドテーブルに出るまでイテレータを進めて見る。
	int h = position;
	while ( h < mCapacity ){
		h = mNexts[ h ];
	}
	//一旦範囲外に出た。ここからmCapacityを引くと次のハッシュ値がわかる。
	h -= mCapacity;
	//-1すればこいつのハッシュ値だ
	h -= 1;
	//ハッシュ値がわかったので、テーブルの先頭から辿っていってpを見つける。
	int prev = mCapacity + h; //ヘッド
	int p = mNexts[ prev ];
	while ( p < mCapacity ){
		if ( p == position ){ //発見。こいつを消す。
			mNexts[ prev ] = mNexts[ p ]; //つなぎ変えて
			mKeys[ p ].~K(); //デストラクト
			mValues[ p ].~V(); //デストラクト
			mEmptyStack.push( p ); //空きスタックにプッシュ
			--mSize;
			return true;
		}
		prev = p;
		p = mNexts[ p ];
	}
	return false;
}

template< class K, class V, class H > inline const V* HashMultiMap< K, V, H >::value( int p ) const {
	return &mValues[ p ];
}

template< class K, class V, class H > inline V* HashMultiMap< K, V, H >::value( int p ){
	return &mValues[ p ];
}

template< class K, class V, class H > inline const K* HashMultiMap< K, V, H >::key( int p ) const {
	return &mKeys[ p ];
}

template< class K, class V, class H > inline void HashMultiMap< K, V, H >::clear(){
	//デストラクトして回る
	int prev = mCapacity; //ヘッド
	int p = mNexts[ mCapacity ]; //ダミーヘッド
	while ( p < mCapacity + mTableSize ){  //mCapacity + mTableSizeは存在しない。そこまではダミーとして存在する。
		if ( p < mCapacity ){
			mNexts[ prev ] = mNexts[ p ]; //つなぎ変えて
			mKeys[ p ].~K();
			mValues[ p ].~V();
			mEmptyStack.push( p ); //空きスタックにプッシュ
		}
		prev = p;
		p = mNexts[ p ];
	}
	mSize = 0;
}

template< class K, class V, class H > inline int HashMultiMap< K, V, H >::next( int p ) const {
	ASSERT( p >= 0 );
	while ( p < mCapacity + mTableSize ){
		p = mNexts[ p ];
		if ( p < mCapacity ){ //ダミーノード以外を返す
			return p;
		}
	}
	return p;
}

template< class K, class V, class H > inline int HashMultiMap< K, V, H >::first() const {
	int p = mCapacity; //最初のダミー
	while ( p < mCapacity + mTableSize ){
		p = mNexts[ p ];
		if ( p < mCapacity ){ //ダミーノード以外を返す
			return p;
		}
	}
	return mCapacity + mTableSize;
}

template< class K, class V, class H > inline bool HashMultiMap< K, V, H >::isEnd( int position ) const {
	return ( position == mCapacity + mTableSize );
}

template< class K, class V, class H > inline void HashMultiMap< K, V, H >::copyTo( V* a ) const {
	int p = mNexts[ mCapacity ];
	int idx = 0;
	while ( p != mCapacity + mTableSize ){
		if ( p < mCapacity ){
			a[ idx ] = mValues[ p ];
			++idx;
		}
		p = mNexts[ p ];
	}
}


template< class K, class V, class H > inline void HashMultiMap< K, V, H >::copyTo( Array< V >* a ) const {
	a->setSize( mSize );
	if ( mSize > 0 ){
		copyTo( &( ( *a )[ 0 ] ) );
	}
}
		


template< class K, class V, class H > inline int HashMultiMap< K, V, H >::size() const {
	return mSize;
}

} //namespace GameLib

#endif
