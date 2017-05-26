#include "GameLib/GameLib.h"
#include "GameLib/Math/Matrix23.h"
#include "GameLib/Math/Matrix22.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Math/Vector2.h"

namespace GameLib{
namespace Math{

Matrix23::Matrix23(){
}

void Matrix23::setIdentity(){
	m00 = m11 = 1.f;
	m01 = m02 = m10 = m12 = 0.f;
}

void Matrix23::setTranslation( float x, float y ){
	m00 = m11 = 1.f;
	m02 = x;
	m12 = y;
	m01 = m10 = 0.f;
}

void Matrix23::setTranslation( const Vector2& a ){
	m00 = m11 = 1.f;
	m02 = a.x;
	m12 = a.y;
	m01 = m10 = 0.f;
}

void Matrix23::setRotation( float r ){
	float s = sin( r );
	float c = cos( r );
	m00 = m11 = c;
	m01 = -s;
	m10 = s;
	m02 = m12 = 0.f;
}

void Matrix23::setScaling( float xy ){
	m00 = m11 = xy;
	m01 = m02 = m10 = m12 = 0.f;
}

void Matrix23::setScaling( float x, float y ){
	m00 = x;
	m11 = y;
	m01 = m02 = m10 = m12 = 0.f;
}

void Matrix23::setScaling( const Vector2& a ){
	m00 = a.x;
	m11 = a.y;
	m01 = m02 = m10 = m12 = 0.f;
}

void Matrix23::translate( float x, float y ){
	m02 += m00 * x + m01 * y;
	m12 += m10 * x + m11 * y;
}

void Matrix23::translate( const Vector2& a ){
	m02 += m00 * a.x + m01 * a.y;
	m12 += m10 * a.x + m11 * a.y;
}


void Matrix23::rotate( float r ){
	float s = sin( r );
	float c = cos( r );
	float t = m00;
	m00 = t * c + m01 * s;
	m01 = t * ( -s ) + m01 * c;
	t = m10;
	m10 = t * c + m11 * s;
	m11 = t * ( -s ) + m11 * c;
}

void Matrix23::scale( float xy ){
	m00 *= xy;
	m01 *= xy;
	m10 *= xy;
	m11 *= xy;
}

void Matrix23::scale( float x, float y ){
	m00 *= x;
	m01 *= y;
	m10 *= x;
	m11 *= y;
}

void Matrix23::scale( const Vector2& a ){
	m00 *= a.x;
	m01 *= a.y;
	m10 *= a.x;
	m11 *= a.y;
}

void Matrix23::mul( Vector2* out, const Vector2& in ) const {
	float tx = in.x;
	out->x = m00 * tx + m01 * in.y + m02;
	out->y = m10 * tx + m11 * in.y + m12;
}

void Matrix23::operator*=( const Matrix23& a ){
	float tx = m00;
	float ty = m01;
	m00 = tx * a.m00 + ty * a.m10;
	m01 = tx * a.m01 + ty * a.m11;
	m02 += tx * a.m02 + ty * a.m12;
	tx = m10;
	tx = m11;
	m10 = tx * a.m00 + ty * a.m10;
	m11 = tx * a.m01 + ty * a.m11;
	m12 += tx * a.m02 + ty * a.m12;
}

void Matrix23::operator*=( const Matrix22& a ){
	float tx = m00;
	m00 = tx * a.m00 + m01 * a.m10;
	m01 = tx * a.m01 + m01 * a.m11;
	tx = m10;
	m10 = tx * a.m00 + m11 * a.m10;
	m11 = tx * a.m01 + m11 * a.m11;
}

void Matrix23::setMul( const Matrix23& a, const Matrix23& b ){
	//a==*this, b==*this両方を考えないといけない
	float t00, t01, t02;
	t00 = a.m00 * b.m00 + a.m01 * b.m10;
	t01 = a.m00 * b.m01 + a.m01 * b.m11;
	t02 = a.m00 * b.m02 + a.m01 * b.m12 + a.m02;

	float t10, t11;
	t10 = a.m10 * b.m00 + a.m11 * b.m10;
	t11 = a.m10 * b.m01 + a.m11 * b.m11;
	m12 = a.m10 * b.m02 + a.m11 * b.m12 + a.m12;

	m00 = t00; m01 = t01; m02 = t02;
	m10 = t10; m11 = t11;
}

void Matrix23::setMul( const Matrix23& a, const Matrix22& b ){
	float tx = a.m00;
	m00 = tx * b.m00 + a.m01 * b.m10;
	m01 = tx * b.m01 + a.m01 * b.m11;
	m02 = a.m02;
	tx = a.m10;
	m10 = tx * b.m00 + a.m11 * b.m10;
	m11 = tx * b.m01 + a.m11 * b.m11;
	m12 = a.m12;
}

float& Matrix23::operator()( int r, int c ){
	ASSERT( r >= 0 && r < 2 );
	ASSERT( c >= 0 && c < 3 );
	return ( &m00 )[ 3 * r + c ];
}

float Matrix23::operator()( int r, int c ) const {
	ASSERT( r >= 0 && r < 2 );
	ASSERT( c >= 0 && c < 3 );
	return ( &m00 )[ 3 * r + c ];
}

void Matrix23::transpose22(){
	float t = m01;
	m01 = m10;
	m10 = t;
	m02 = m12 = 0.f;
}

void Matrix23::setTransposed22( const Matrix23& a ){
	m00 = a.m00;
	m01 = a.m10;
	m10 = a.m01;
	m11 = a.m11;
	m02 = m12 = 0.f;
}



//クラーメル法での逆行列
/*
まず2x3行列というのは
2x2部分をかけたあと、あるベクタで移動する行列だった。

q = Ap + b

この逆行列というのは、
ベクタの逆向きで移動してから、2x2部分Aの逆行列をかけたものになる。
つまり、

p = A'(q - b)

だ。展開すると、

p = A'q - A'b

となる。要するに、A'をかけた後で足すベクタは-A'bで、
これが2x3行列に入っている移動ベクタだ。

馬鹿正直に3x3に拡張してやる必要はないということである。

ちなみに2x2部分はad-bc=Dと置けば(行列式)

x = d/D
y = -b/D
z = -c/D
w = a/D

である。詳細はMatrix22.cppを参照のこと
*/
void Matrix23::setInverse( const Matrix23& a ){
	//まず2x2部分
	float delta = a.m00 * a.m11 - a.m01 * a.m10;
	float rcpDelta = 1.f / delta;
	float t = a.m00; //バックアップ(a==*thisに備える)
	m00 = a.m11 * rcpDelta;
	m01 = -a.m01 * rcpDelta;
	m10 = -a.m10 * rcpDelta;
	m11 = t * rcpDelta;

	//次に移動成分。できた2x2逆行列を元の移動ベクタにかけてやればいい
	t = a.m02;
	m02 = -( m00 * t + m01 * a.m12 );
	m12 = -( m10 * t + m11 * a.m12 );
}

void Matrix23::invert(){ 
	setInverse( *this );
}

bool Matrix23::operator==( const Matrix23& a ) const {
	for ( int i = 0; i < 2; ++i ){
		for ( int j = 0; j < 3; ++j ){
			if ( ( *this )( i, j ) != a( i, j ) ){
				return false;
			}
		}
	}
	return true;
}

bool Matrix23::operator!=( const Matrix23& a ) const {
	return !( operator==( a ) );
}


} //namespace Math
} //namespace GameLib
