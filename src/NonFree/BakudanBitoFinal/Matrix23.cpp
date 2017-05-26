#include "Matrix23.h"
#include "Vector2.h"
#include "GameLib/Math.h"
using namespace GameLib;

Matrix23::Matrix23(){
}

void Matrix23::setTranslation( const Vector2& a ){
	m00 = m11 = 1.0;
	m01 = m10 = 0.0;
	m02 = a.x;
	m12 = a.y;
}

void Matrix23::setRotation( double r ){
	double s = GameLib::sin( r );
	double c = GameLib::cos( r );
	m00 = m11 = c;
	m01 = s;
	m10 = -s;
	m02 = m12 = 0.0;
}

void Matrix23::setScaling( const Vector2& a ){
	m00 = a.x;
	m11 = a.y;
	m01 = m10 = m02 = m12 = 0.0;
}

void Matrix23::operator*=( const Matrix23& m ){
	double tx, ty;
	tx = m00; ty = m01;
	m00 = tx * m.m00 + ty * m.m10;
	m01 = tx * m.m01 + ty * m.m11;
	m02 = tx * m.m02 + ty * m.m12 + m02;
	tx = m10; ty = m11;
	m10 = tx * m.m00 + ty * m.m10;
	m11 = tx * m.m01 + ty * m.m11;
	m12 = tx * m.m02 + ty * m.m12 + m12;
}

void Matrix23::multiply( Vector2* out, const Vector2& in ) const {
	double tx = in.x; //outとinが同じかもしれないので、バックアップ
	out->x = m00 * tx + m01 * in.y + m02; //左上*x + 中上*y + 右上*1
	out->y = m10 * tx + m11 * in.y + m12; //左下*x + 中下*y + 左下*1
}

//無駄のない移動行列乗算関数の作り方
/*
a b c
d e f

に

1 0 tx
0 1 ty

を掛ける。3x3に拡張すれば、

a b c   1 0 tx
d e f   0 1 ty
0 0 1   0 0 1

3行目はどうでもいいので上2行だけ計算すると、

a b (a*tx + b*ty + c)
d e (d*tx + e*ty + f)

のことだ。これが答えである。
つまり、m02とm12しか触らなくていい
*/

void Matrix23::translate( const Vector2& a ){
	m02 += m00 * a.x + m01 * a.y; 
	m12 += m10 * a.x + m11 * a.y;
}

//無駄のない拡大縮小行列乗算の作り方
/*
a b c
d e f

に

sx 0 0
0 sy 0

を掛ける。

a b c   sx 0  0
d e f   0  sy 0
0 0 1   0  0  1

の上2行を計算すれば、

a*sx b*sy c
d*sx e*sy f

これが答えである。
m00,m01,m10,m11の4要素を触ればいい
*/
void Matrix23::scale( const Vector2& a ){
	m00 *= a.x;
	m01 *= a.y;
	m10 *= a.x;
	m11 *= a.y;
}

//無駄のない回転行列乗算の作り方
/*
a b c
d e f

に

p -q 0
q p 0

を掛ける。pがコサインで、qがサインだ。

a b c   p -q 0
d e f   q  p 0
0 0 1   0  0 1

上2行を計算すると、

(ap + bq) (-aq + bp) c
(dp + eq) (-dq + ep) f

m00,m01,m10,m11の4要素を触ればいい
*/

void Matrix23::rotate( double r ){
	double c = GameLib::cos( r );
	double s = GameLib::sin( r );
	//m00はバックアップを取ります。一行目でm00を上書きしてしまうので。
	double t = m00;
	m00 = t * c + m01 * s;
	m01 = t * ( -s ) + m01 * c;
	t = m10; //m10をバックアップ
	m10 = t * c + m11 * s;
	m11 = t * ( -s ) + m11 * c;
}
