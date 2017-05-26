#ifndef INCLUDED_VECTOR3_H
#define INCLUDED_VECTOR3_H

class Vector3{
public:
	//何もしないコンストラクタ
	Vector3();
	//doubleからコンストラクタ
	Vector3( double x, double y, double z );
	//Vector2からコンストラクタ
	Vector3( const Vector3& );
	//後から数字をセット
	void set( double x, double y, double z );
	//足し算
	void operator+=( const Vector3& );
	//引き算
	void operator-=( const Vector3& );
	//コピー
	void operator=( const Vector3& );
	//足し算をセット
	void setAdd( const Vector3&, const Vector3& );
	//引き算をセット
	void setSub( const Vector3&, const Vector3& );
	//xyzをa倍
	void operator*=( double );
	//掛け算をセット
	void setMul( const Vector3&, double );
	//長さの二乗ゲット
	double squareLength() const;
	//内積計算
	double dot( const Vector3& ) const;
	///this = crossProduct( a, b )
	void setCross( const Vector3& a, const Vector3& b );
	/// this = a * b + c
	void setMadd( const Vector3& a, double b, const Vector3& c );

	double x, y, z;
};

#endif
