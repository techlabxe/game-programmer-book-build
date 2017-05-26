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
	//足し算をセット
	void setAdd( const Vector2&, const Vector2& );
	//引き算をセット
	void setSub( const Vector2&, const Vector2& );
	//補間をセット( this = a + u*ab + v*ac )
	void setInterporation( 
		const Vector2& a, 
		const Vector2& ab, 
		const Vector2& ac, 
		double u, 
		double v );

	double x;
	double y;
};

#endif
