#ifndef INCLUDED_GAMELIB_TANK_H
#define INCLUDED_GAMELIB_TANK_H

#include "GameLib/Base/Array.h"

namespace GameLib{

///足し終わるまでアクセスせず、いくつ足すかがわからない、という用途のためのもの
/*!
水をためて一気に流すイメージでタンクとしたが、もっといい名前があれば募集。
内部的には容量単位(blockSize)づつ内部でアロケートして継ぎ足していく。

アクセスはgetCurrent()で取得、toNext()で次へ。rewind()で最初からアクセスしなおせる。
*/
template< class T > class Tank{
public:
	explicit Tank( int blockSize = 16 );
	~Tank();
	///再確保。中身がある状態で呼ぶとassert。clear()しろ。
	void setBlockSize( int blockSize );
	///現在の要素数取得
	int size() const;
	///全解放
	void clear();
	///末尾に空で追加。足したものへのポインタを返すので、初期設定はこれを使うこと。
	T* add();
	///末尾に追加
	void add( const T& );
	///次へ
	void toNext();
	///終わりです
	bool isEnd() const;
	///現在の要素を取得(非const)
	T* get();
	///現在の要素を取得(const)
	const T* get() const;
	///最初の要素にポインタを戻す
	void rewind();
	///単なる配列に変換。前もってsize()したサイズでnewした領域を渡すこと。
	void copyTo( T* ) const;
	///Arrayにコピー。空でないと怒られる。
	void copyTo( Array< T >* ) const;
private:
	struct Block{
		Block* mNext; //次のノード
		T* mElements; //配列
	};
	void operator=( const Tank& ); //代入禁止
	Tank( const Tank& ); //コピーコンストラクタ禁止

	//定数
	int mBlockSize; //単位量
	int mSize;
	Block mHead; //ダミー

	Block* mLastBlock; //最終ノード
	int mLastPos; //最終ノード内の位置
	Block* mCurrentBlock; //現在のイテレータ
	int mCurrentPos; //ノード内位置
};

} //namespace GameLib
#include "GameLib/Base/Impl/TankImpl.h" //中身はこの中

#endif
