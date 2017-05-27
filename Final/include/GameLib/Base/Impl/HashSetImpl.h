#ifndef INCLUDED_GAMELIB_HASTSETIMPL_H
#define INCLUDED_GAMELIB_HASHSETIMPL_H

#include "GameLib/GameLib.h"
#include "GameLib/Base/HashSet.h"
#include "GameLib/Base/PrimeNumber.h"

/*
<注意>
添え字でmCapacity + i (0<=i<mTableSize)が使われていたら、それはダミーノードである。
mNodes[ mCapacity + i ].mNextはmNodes[ mCapacity + i + 1 ]をさしている。
*/

namespace GameLib{

//先頭と末尾が特別扱いされないようにちょっと工夫をしている。
template< class T, class H > inline HashSet< T, H >::HashSet() : 
mNexts( 0 ),
mValues( 0 ),
mCapacity( 0 ),
mSize( 0 ),
mTableSize( 0 ){
	ASSERT( sizeof( T ) != 0 ); //ありえないよね？
}

//先頭と末尾が特別扱いされないようにちょっと工夫をしている。
template< class T, class H > inline HashSet< T ,H >::HashSet( int capacity, int tableSize ) : 
mNexts( 0 ),
mValues( 0 ),
mCapacity( 0 ),
mSize( 0 ),
mTableSize( 0 ){
	ASSERT( sizeof( T ) != 0 ); //ありえないよね？
	setCapacity( capacity, tableSize );
}

template< class T, class H > inline HashSet< T, H >::~HashSet(){
	if ( mNexts ){
		int pos = mNexts[ mCapacity ];
		while ( pos < mCapacity + mTableSize ){ //ノードがあれば
			if ( pos < mCapacity ){
				mValues[ pos ].~T();
			}
			pos = mNexts[ pos ];
		}
		OPERATOR_DELETE( mValues ); //生delete
		SAFE_DELETE_ARRAY( mNexts );
	}
}

template< class T, class H > inline void HashSet< T, H >::setCapacity( int capacity, int tableSize ){
	ASSERT( mSize == 0 && "NOT EMPTY! call clear()." );
	//まずクリア
	if ( mCapacity > 0 ){
		mEmptyStack.clear();
		clear();
		OPERATOR_DELETE( mValues ); //生delete
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
	}else if ( tableSize > 0x10000 ){ //65536以上は許さない
		tableSize = 65521; //65536以下最大の素数
	}
	mTableSize = tableSize;
	//次配列 テーブルサイズ分だけダミーを足すことに注意
	mNexts = NEW int[ mCapacity + mTableSize ];
	//値配列確保。コンストラクタが不要なので生new
	mValues = static_cast< T* >( OPERATOR_NEW( sizeof( T ) * mCapacity ) );
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

template< class T, class H > inline int HashSet< T, H >::capacity() const {
	return mCapacity;
}

template< class T, class H > inline int HashSet< T, H >::add( const T& v ){
	ASSERT( mSize < mCapacity );
	//ハッシュ関数算出
	int h = H().value( v, mTableSize );
	//検索
	int p = mNexts[ mCapacity + h ];
	while ( p < mCapacity ){
		if ( H().isEqual( mValues[ p ], v ) ){
			break;
		}
		p = mNexts[ p ];
	}
	if ( p >= mCapacity ){ //ないから足す
		//空き場所を取得
		int newPos;
		mEmptyStack.pop( &newPos );
		//テーブル先頭に挿入
		int head = mCapacity + h;
		mNexts[ newPos ] = mNexts[ head ]; //頭ダミーの次をセット
		new ( &mValues[ newPos ] ) T( v ); //コピーコンストラクタ呼び出し
		mNexts[ head ] = newPos; //前のリンクを新ノードにつなぐ
		++mSize;
		return newPos;
	}else{
		return mCapacity + mTableSize;
	}
}

template< class T, class H > inline int HashSet< T, H >::find( const T& v ){
	int h = H().value( v, mTableSize );

	int p = mNexts[ mCapacity + h ];
	while ( p < mCapacity ){
		if ( H().isEqual( mValues[ p ], v ) ){
			return p;
		}
		p = mNexts[ p ];
	}
	return mCapacity + mTableSize;
}

template< class T, class H > inline bool HashSet< T, H >::isIn( const T& v ) const {
	int h = H().value( v, mTableSize );

	int p = mNexts[ mCapacity + h ];
	while ( p < mCapacity ){
		if ( H().isEqual( mValues[ p ], v ) ){
			return true;
		}
		p = mNexts[ p ];
	}
	return false;
}

template< class T, class H > inline bool HashSet< T, H >::remove( int position ){
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
			mValues[ p ].~T(); //デストラクト
			mEmptyStack.push( p ); //空きスタックにプッシュ
			--mSize;
			return true;
		}
		prev = p;
		p = mNexts[ p ];
	}
	return false;
}

template< class T, class H > inline const T* HashSet< T, H >::value( int p ) const {
	return &mValues[ p ];
}

template< class T, class H > inline void HashSet< T, H >::clear(){
	//デストラクトして回る
	int prev = mCapacity; //ヘッド
	int p = mNexts[ mCapacity ]; //ダミーヘッド
	while ( p < mCapacity + mTableSize ){  //mCapacity + mTableSizeは存在しない。そこまではダミーとして存在する。
		if ( p < mCapacity ){
			mNexts[ prev ] = mNexts[ p ]; //つなぎ変えて
			mValues[ p ].~T();
			mEmptyStack.push( p ); //空きスタックにプッシュ
		}
		prev = p;
		p = mNexts[ p ];
	}
	mSize = 0;
}

template< class T, class H > inline int HashSet< T, H >::next( int p ) const {
	ASSERT( p >= 0 );
	while ( p < mCapacity + mTableSize ){
		p = mNexts[ p ];
		if ( p < mCapacity ){ //ダミーノード以外を返す
			return p;
		}
	}
	return p;
}

template< class T, class H > inline int HashSet< T, H >::first() const {
	int p = mCapacity; //最初のダミー
	while ( p < mCapacity + mTableSize ){
		p = mNexts[ p ];
		if ( p < mCapacity ){ //ダミーノード以外を返す
			return p;
		}
	}
	return mCapacity + mTableSize;
}

template< class T, class H > inline bool HashSet< T, H >::isEnd( int position ) const {
	return ( position == mCapacity + mTableSize );
}

template< class T, class H > inline void HashSet< T, H >::copyTo( T* a ) const {
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

template< class T, class H > inline void HashSet< T, H >::copyTo( Array< T >* a ) const {
	a->setSize( mSize );
	if ( mSize > 0 ){
		copyTo( &( ( *a )[ 0 ] ) );
	}
}


template< class T, class H > inline int HashSet< T, H >::size() const {
	return mSize;
}

} //namespace GameLib

#endif
