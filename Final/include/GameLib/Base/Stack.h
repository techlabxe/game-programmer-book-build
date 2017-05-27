#ifndef INCLUDED_GAMELIB_STACK_H
#define INCLUDED_GAMELIB_STACK_H

namespace GameLib{

///素直な固定サイズスタック
template< class T > class Stack{
public:
	///デフォルト。サイズ、最大容量は0。
	Stack();
	///最大容量を指定。
	explicit Stack( int capacity );
	~Stack();
	///再確保。中身がある状態で呼ぶとassert。clear()しろ。
	void setCapacity( int size );
	///最大容量取得
	int capacity() const;
	///現在の要素数取得
	int size() const;
	///全解放
	void clear();
	///末尾に追加
	void push( const T& );
	///末尾に追加(デフォルトコンストラクタ呼び出し)
	void push();
	///末尾を削除
	void pop();
	///末尾を削除(捨てる要素のコピーを受け取る)
	void pop( T* out );
	///末尾を取得(const)
	const T* get() const;
	///末尾を取得(非const)
	T* get();
private:
	void operator=( const Stack& ); //代入禁止
	Stack( const Stack& ); //コピーコンストラクタ禁止

	T* mElements;
	int mSize;
	int mCapacity;
	int mBack;
};

} //namespace GameLib
#include "GameLib/Base/Impl/StackImpl.h" //中身はこの中

#endif

