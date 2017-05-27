#ifndef INCLUDED_GAMELIB_LIST_H
#define INCLUDED_GAMELIB_LIST_H

#include "GameLib/Base/Stack.h"
#include "GameLib/Base/Array.h"

namespace GameLib{

///newのオーバーへッドに耐えられないせっかちさんのための双方向リスト
template< class T > class List{
public:
	List();
	explicit List( int capacity );
	~List();
	///後からサイズ指定。中身があると死ぬ。clear()しよう。
	void setCapacity( int capacity );
	int capacity() const;
	//追加 全て場所を返す
	int addBefore( int position, const T& value ); //指定場所の前に追加
	int addAfter( int position, const T& value ); //指定場所の後に追加
	int addHead( const T& value ); //先頭追加
	int addTail( const T& value ); //末尾追加
	//削除
	void remove( int position ); //指定場所を削除
	void removeHead(); //先頭を削除
	void removeTail(); //末尾を削除
	void clear(); //全部削除
	///値取得(const)
	const T* value( int position ) const;
	///値取得(非const)
	T* value( int position );
	///末端かどうかはisEnd()で確認せよ
	int next( int position ) const;
	///末端かどうかはisEnd()で確認せよ
	int previous( int position ) const;
	int first() const;
	int last() const;
	///positionが末端か判定。
	bool isEnd( int position ) const;
	///単なる配列に変換。前もってsize()したサイズでnewした領域を渡すこと。
	void copyTo( T* ) const;
	///Arrayにコピー。空のものを渡せ。
	void copyTo( Array< T >* ) const;
private:
	struct Node{
		int mNext;
		int mPrev;
	};
	List( const List& ); //コピーコンストラクタ禁止
	void operator=( const List& ); //代入禁止

	T* mValues;
	Node* mNodes;
	Stack< int > mEmptyStack;
	int mCapacity;
	int mSize;
};

} //namespace GameLib
#include "GameLib/Base/Impl/ListImpl.h" //中身はこの中

#endif

