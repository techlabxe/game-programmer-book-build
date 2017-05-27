#ifndef INCLUDED_GAMELIB_POOL_H
#define INCLUDED_GAMELIB_POOL_H

namespace GameLib{

///ある型の置き場所。もっぱらnewを削減するためだけに用いられる。
/*!
確保するためだけにあり、その後の操作は何もできないことに注意すること。
また、部分解放も不可能。できるのは全体解放だけである。
*/
template< class T > class Pool{
public:
	explicit Pool( int defaultBlockSize = 16 );
	~Pool();
	///標準ブロック数を変更。
	void setDefaultBlockSize( int blockSize );
	///全解放
	void clear();
	///n個確保
	T* allocate( int n = 1 );
	///n個コピーコンストラクト
	T* allocate( const T*, int n = 1 );
private:
	struct Block{
		Block* mNext; //次のブロック
		T* mElements; //配列
		int mUsedNumber; //使われた数
		int mSize; //ブロックのサイズ
	};
	void operator=( const Pool& ); //代入禁止
	Pool( const Pool& ); //コピーコンストラクタ禁止

	//定数
	int mDefaultBlockSize; //単位量
	Block mHead; //ダミーノード。
	Block* mLastBlock; //最終ノード
};

} //namespace GameLib
#include "GameLib/Base/Impl/PoolImpl.h" //中身はこの中

#endif
