#ifndef INCLUDED_GAMELIB_VECTOR_H
#define INCLUDED_GAMELIB_VECTOR_H

namespace GameLib{

///低機能版std::vector
/*!
std::vectorを使わずこちらを使う利点はなにか？
実はあまりない。頻度の低い関数や、危険な関数を除いてあるので
使いやすく、学習には適しているだろう。
あとはSTLでsize_tが返ってくることに閉口している人にはお勧めである。intでいいじゃん！

基本的にこのクラスは楽に書くためにあり、サイズ変更系の関数を
呼べば当然遅い。Arrayと違ってコピーもできるが、無思慮なコピーは破滅的な性能の劣化を
招くだろう。
*/
template< class T > class Vector{
public:
	///capacityを指定すると、その数までは増えても引っ越さない。
	explicit Vector( int size = 0, int capacity = 0 );
	///丸ごとコピーする。遅いことは覚悟せよ
	Vector( const Vector& ); //コピーコンストラクタを禁止しない
	~Vector();
	//代入。丸ごとコピーする。遅いことは覚悟せよ
	void operator=( const Vector& ); //代入も禁止しない
	///追加
	void push( const T& );
	///場所だけ作る追加。ポインタを返すのでここに入れること。
	T* push();
	///最終要素を削除
	void pop();
	///全削除
	void clear();
	///サイズ取得
	int size() const;
	///サイズ変更。小さくしても引越しが起こらない。
	void setSize( int );
	///添え字(非const)
	T& operator[]( int );
	///添え字(const)
	const T& operator[]( int ) const;
	///中身を別のベクタに移す。自分は空になる。相手に入っていたものは消える。
	void moveTo( Vector* );
private:
	T* mElements;
	int mSize;
	int mCapacity;
};

} //namespace GameLib
#include "GameLib/Base/Impl/VectorImpl.h" //中身はこの中

#endif

