#ifndef INCLUDED_GAMELIB_ARRAY_H
#define INCLUDED_GAMELIB_ARRAY_H

namespace GameLib{

///解放を忘れがちなあわてんぼさんのための自動解放配列
template< class T > class Array{
public:
	///デフォルト。サイズは0。
	Array();
	///初期サイズ。全要素はデフォルトコンストラクタで初期化される。
	explicit Array( int size );
	//初期サイズ、初期値を渡す
	explicit Array( int size, const T& );
	~Array();
	///後から確保。縮小は可能だが、拡大は不可。一旦clear()すること。
	void setSize( int size );
	///サイズ取得
	int size() const;
	///全解放
	void clear();
	///添え字(const)
	const T& operator[]( int i ) const;
	///添え字(非const)
	T& operator[]( int i );
private:
	void operator=( const Array& ); //代入禁止
	Array( const Array& ); //コピーコンストラクタ禁止

	T* mElements;
	int mSize;
};

} //namespace GameLib
#include "GameLib/Base/Impl/ArrayImpl.h" //中身はこの中

#endif

