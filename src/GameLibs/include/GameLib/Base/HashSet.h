#ifndef INCLUDED_GAMELIB_HASHSET_H
#define INCLUDED_GAMELIB_HASHSET_H

#include "GameLib/Base/Stack.h"
#include "GameLib/Base/Hash.h"
#include "GameLib/Base/Array.h"

namespace GameLib{

///限界容量指定ハッシュテーブル
/*!
ハッシュ関数は全ビットの和をテーブルサイズで剰余を取る単純な方式である。
全ビットであるため、クラスのサイズが大きいとコストが大きくなる可能性が高い。
なお、同一要素は挿入しない。std::mutli_setが必要になる例を見たことがないからだ。
*/
template< class T, class H = Hash< T > > class HashSet{
public:
	///デフォルト。setCapacityを呼ばないと動かない
	HashSet();
	///最大容量を指定。テーブルサイズはデフォルトでcapacity以上の最小の素数。
	explicit HashSet( int capacity, int tableSize = 0 );
	~HashSet();
	///再確保。中身がある状態で呼ぶとassert。clear()しろ。
	void setCapacity( int capacity, int tableSize = 0 );
	///最大容量取得
	int capacity() const;
	///現在の要素数取得
	int size() const;
	///全解放
	void clear();
	///追加。追加した場所を返す。同じものがすでにあれば無視して無効イテレータを返す。
	int add( const T& );
	///検索。返るのはイテレータ
	int find( const T& );
	///検索。ただし、あるかどうかだけ返す
	bool isIn( const T& ) const;
	///イテレータを指定して削除。削除すればtrueを返す。
	bool remove( int position );
	///値取得。書き換えは許さないのでconst版しか提供しない。
	const T* value( int position ) const;
	//以下単純なリストとしてアクセスする際のインターフェイス。ただし片方向
	int next( int position ) const;
	int first() const;
	///positionが末端か判定。
	bool isEnd( int position ) const;
	///単なる配列に変換。前もってsize()したサイズでnewした領域を渡すこと。
	void copyTo( T* ) const;
	///Arrayにコピー。空のものを渡せ。
	void copyTo( Array< T >* ) const;
private:
	void operator=( const HashSet& ); //代入禁止
	HashSet( const HashSet& ); //コピーコンストラクタ禁止

	int* mNexts;
	T* mValues;
	Stack< int > mEmptyStack;
	int mCapacity;
	int mSize;
	int mTableSize;
};

} //namespace GameLib
#include "GameLib/Base/Impl/HashSetImpl.h" //中身はこの中

#endif

