#ifndef INCLUDED_GAMELIB_QUEUE_H
#define INCLUDED_GAMELIB_QUEUE_H

namespace GameLib{

///キュー。要素数が限界を超えなければ循環するのでいつまでも使える。
template< class T > class Queue{
public:
	///デフォルト。サイズ、最大容量は0。
	Queue();
	///最大容量を指定。
	explicit Queue( int capacity );
	~Queue();
	///再確保。中身がある状態で呼ぶとassert。clear()しろ。
	void setCapacity( int size );
	///最大容量取得
	int capacity() const;
	///現在の要素数取得
	int size() const;
	///全解放
	void clear();
	///末尾に空で追加。足したものへのポインタを返すので、初期設定はこれを使うこと。
	T* pushBack();
	///末尾に追加
	void pushBack( const T& );
	///先頭を削除
	void popFront();
	///先頭を削除(捨てるもののコピーを取得)
	void popFront( T* out );
	///先頭を取得(const)
	const T* front() const;
	///先頭を取得(非const)
	T* front();
private:
	void operator=( const Queue& ); //代入禁止
	Queue( const Queue& ); //コピーコンストラクタ禁止

	T* mElements;
	int mSize;
	int mCapacity;
	int mFront;
};

} //namespace GameLib
#include "GameLib/Base/Impl/QueueImpl.h" //中身はこの中

#endif

