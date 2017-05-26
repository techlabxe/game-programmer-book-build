#ifndef INCLUDED_GAMELIB_HASHMULTIMAP_H
#define INCLUDED_GAMELIB_HASHMULTIMAP_H

#include "GameLib/Base/Stack.h"
#include "GameLib/Base/Hash.h"
#include "GameLib/Base/Array.h"

namespace GameLib{

///限界容量指定ハッシュテーブル
/*!
ハッシュ関数は全ビットの和をテーブルサイズで剰余を取る単純な方式である。
全ビットであるため、クラスのサイズが大きいとコストが大きくなる可能性が高い。
なお、同一キーをもつ要素も追加する。findは同じキーを持つものの先頭を返す。
そこからgetNextで異なるキーのものが返るまでループで回すこと。
ただし、必ずisEnd()を呼んで範囲外でないことを確認すること。
*/
template< class Key, class Value, class H = Hash< Key > > class HashMultiMap{
public:
	///デフォルト。setCapacityを呼ばないと動かない
	HashMultiMap();
	///最大容量を指定。テーブルサイズはデフォルトでcapacity以上の最小の素数。
	explicit HashMultiMap( int capacity, int tableSize = 0 );
	~HashMultiMap();
	///再確保。中身がある状態で呼ぶとassert。clear()しろ。
	void setCapacity( int capacity, int tableSize = 0 );
	///最大容量取得
	int capacity() const;
	///現在の要素数取得
	int size() const;
	///全解放
	void clear();
	///追加。追加した場所を返す。
	int add( const Key&, const Value& );
	///追加。追加した場所を返す。値はgetValueして後から書き込むこと。
	int add( const Key& );
	///検索。返るのはイテレータ。同一キーの先頭。
	int find( const Key& ) const;
	///イテレータを指定して削除。削除すればtrueを返す。
	bool remove( int position );
	///キー取得(書き換えは禁止なのでconstのみ)
	const Key* key( int position ) const;
	///値取得(const)
	const Value* value( int position ) const;
	///値取得(非const)
	Value* value( int position );
	//以下単純なリストとしてアクセスする際のインターフェイス。ただし片方向
	int next( int position ) const;
	int first() const;
	///positionが末端か判定。
	bool isEnd( int position ) const;
	///単なる配列に変換。前もってsize()したサイズでnewした領域を渡すこと。
	void copyTo( Value* ) const;
	///Arrayにコピー。空のものを渡せ。
	void copyTo( Array< Value >* ) const;
private:
	void operator=( const HashMultiMap& ); //代入禁止
	HashMultiMap( const HashMultiMap& ); //コピーコンストラクタ禁止

	int* mNexts;
	Key* mKeys;
	Value* mValues;
	Stack< int > mEmptyStack;
	int mCapacity;
	int mSize;
	int mTableSize;
};

} //namespace GameLib
#include "GameLib/Base/Impl/HashMultiMapImpl.h" //中身はこの中

#endif

