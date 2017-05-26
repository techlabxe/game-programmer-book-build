#ifndef INCLUDED_VECTOR2_H
#define INCLUDED_VECTOR2_H

class Vector2{
public:
	//何もしないコンストラクタ
	Vector2();
	//整数からコンストラクタ
	Vector2( int x, int y );
	//doubleからコンストラクタ
	Vector2( double x, double y );
	//Vector2からコンストラクタ
	Vector2( const Vector2& );
	//足し算
	void operator+=( const Vector2& );
	//引き算
	void operator-=( const Vector2& );
	//コピー
	void operator=( const Vector2& );

	double x;
	double y;
};

#endif
