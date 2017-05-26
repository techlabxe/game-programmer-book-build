#include "Sphere.h"
#include "Library/Vector3.h"


void Sphere::setPosition( const Vector3& a ){
	mPosition = a;
}

void Sphere::setRadius( double a ){
	mRadius = a;
}

bool Sphere::isIntersect( const Sphere& b ) const {
	Vector3 difference;
	difference.setSub( mPosition, b.mPosition );
	double rSum = mRadius + b.mRadius;
	double squareDistance = difference.squareLength();
	rSum *= rSum;
	return ( squareDistance < rSum );
}

void Sphere::restrictMove( Vector3* v, const Sphere& a ) const {
	//コピー生成
	Sphere t = *this;
	t.mPosition += *v; //移動
	//移動先で判定して、当たってなければもういい。
	bool r = t.isIntersect( a );
	if ( !r ){
		return;
	}
	//当たっているならベクタを修正する。
	//b = (c0-c1)/|c0-c1|
	//a = v - dot( b, v ) * b
	//bが長さ1の衝突垂直方向ベクタ。aは結果として欲しい水平方向ベクタ。
	//ここで、上の式を下の式にそのまま入れると
	//a = v - dot( (c0-c1)/|c0-c1|, v ) / |c0-c1| * b
	//となる。c0-c1をdとすれば、
	//a = v - dot( d, v ) * d / |d|^2
	//となり、より計算が簡単になる。
	Vector3 d;
	d.setSub( mPosition, a.mPosition ); //c0-c1 = d
	double dotDV = d.dot( *v ); //dot(d, v)
	double dLength2 = d.squareLength(); //|d|^2
	d *= ( dotDV / dLength2 ); //dot(b,v) / |d|^2
	//最後の引き算
	*v -= d;
}
