#ifndef INCLUDED_GAMELIB_ARRAY2D_H
#define INCLUDED_GAMELIB_ARRAY2D_H

namespace GameLib{

///二次元配列
template< class T > class Array2d{
public:
	///デフォルト。サイズは0。
	Array2d();
	///初期サイズ。全要素はデフォルトコンストラクタで初期化される。
	Array2d( int size0, int size1 );
	~Array2d();
	///後から確保。中身があるとASSERT()する。一旦clear()すること。
	void setSize( int size0, int size1 );
	///サイズ取得
	int size( int index ) const;
	///全解放
	void clear();
	///添え字(const)
	const T& operator()( int i, int j ) const;
	///添え字(非const)
	T& operator()( int i, int j );
private:
	void operator=( const Array2d& ); //代入禁止
	Array2d( const Array2d& ); //コピーコンストラクタ禁止

	T* mElements;
	int mSize[ 2 ];
};

} //namespace GameLib
#include "GameLib/Base/Impl/Array2dImpl.h" //中身はこの中

#endif

