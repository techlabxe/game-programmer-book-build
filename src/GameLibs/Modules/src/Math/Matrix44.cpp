#include "GameLib/GameLib.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Vector4.h"

namespace GameLib{
namespace Math{

Matrix44::Matrix44(){
}

void Matrix44::setIdentity(){
	m00 = m11 = m22 = m33 = 1.f;
	m01 = m02 = m03 = 0.f;
	m10 = m12 = m13 = 0.f;
	m20 = m21 = m23 = 0.f;
	m30 = m31 = m32 = 0.f;
}

void Matrix44::setPerspectiveTransform(
float fieldOfViewY,
float windowWidth,
float windowHeight,
float nearClip,
float farClip,
bool zBufferIsLinear ){
	float s = 1.f / tan( fieldOfViewY * 0.5f );
	float r = windowHeight / windowWidth;
	float a;
	if ( zBufferIsLinear ){
		a = 1.f / ( nearClip - farClip );
	}else{
		a = farClip / ( nearClip - farClip );
	}
	float b = nearClip * a;
	m00 = s * r;
	m11 = s;
	m22 = a;
	m23 = b;
	m32 = -1.f;
	//残り全部0
	m01 = m02 = m03 = 0.f;
	m10 = m12 = m13 = 0.f;
	m20 = m21 = 0.f;
	m30 = m31 = m33 = 0.f;
}

/*
[x0, x1] -> [ -1, 1 ]
-1 = x0*a + b
1 = x1*a + b
-2 = (x0-x1)a

a = -2/(x0-x1)
b = 1 - x1*a

Yも同じ

[z0, z1] -> [1, 0]
1 = z0*a + b
0 = z1*a + b

1 = (z0-z1)a

a = 1/(z0-z1)
b = -z1*a

*/
void Matrix44::setOrthogonalTransform(
float xMin,
float xMax,
float yMin,
float yMax,
float zMin,
float zMax ){
	m00 = -2.f / ( xMin - xMax );
	m03 = 1.f - xMax * m00;
	m11 = -2.f / ( yMin - yMax );
	m13 = 1.f - yMax * m11;
	m22 = 1.f / ( zMin - zMax );
	m23 = -zMax * m22;
	m33 = 1.f;
	//残りはゼロ
	m01 = m02 = m10 = m12 = m20 = m21 = 0.f;
	m30 = m31 = m32 = 0.f;
}

void Matrix44::multiplyViewTransform(
const Vector3& p,
const Vector3& t,
const Vector3& u ){
	Vector3 x, y, z;
	z.setSub( p, t ); //注視点からカメラ位置へのベクタをZ軸に
	z.normalize();
	x.setCross( u, z ); //上ベクタとZ軸の外積をX軸に。上ベクタが上向きなら左向きがX軸プラス。
	x.normalize();
	y.setCross( z, x ); //Z,Xの外積がY

	//移動分は今作った回転行列にカメラ座標を乗算してマイナスにすればいい。
	float t03 = -( x.x * p.x + x.y * p.y + x.z * p.z );
	float t13 = -( y.x * p.x + y.y * p.y + y.z * p.z );
	float t23 = -( z.x * p.x + z.y * p.y + z.z * p.z );

	//あとは行列乗算
	float tx = m00;
	float ty = m01;
	float tz = m02;
	m00 = tx * x.x + ty * y.x + tz * z.x;
	m01 = tx * x.y + ty * y.y + tz * z.y;
	m02 = tx * x.z + ty * y.z + tz * z.z;
	m03 += tx * t03 + ty * t13 + tz * t23;
	tx = m10;
	ty = m11;
	tz = m12;
	m10 = tx * x.x + ty * y.x + tz * z.x;
	m11 = tx * x.y + ty * y.y + tz * z.y;
	m12 = tx * x.z + ty * y.z + tz * z.z;
	m13 += tx * t03 + ty * t13 + tz * t23;
	tx = m20;
	ty = m21;
	tz = m22;
	m20 = tx * x.x + ty * y.x + tz * z.x;
	m21 = tx * x.y + ty * y.y + tz * z.y;
	m22 = tx * x.z + ty * y.z + tz * z.z;
	m23 += tx * t03 + ty * t13 + tz * t23;
	tx = m30;
	ty = m31;
	tz = m32;
	m30 = tx * x.x + ty * y.x + tz * z.x;
	m31 = tx * x.y + ty * y.y + tz * z.y;
	m32 = tx * x.z + ty * y.z + tz * z.z;
	m33 += tx * t03 + ty * t13 + tz * t23;
}

void Matrix44::mul( Vector4* out, const Vector3& in ) const {
	out->x = m00 * in.x + m01 * in.y + m02 * in.z + m03;
	out->y = m10 * in.x + m11 * in.y + m12 * in.z + m13;
	out->z = m20 * in.x + m21 * in.y + m22 * in.z + m23;
	out->w = m30 * in.x + m31 * in.y + m32 * in.z + m33;
}

void Matrix44::operator*=( const Matrix34& a ){
	float tx = m00;
	float ty = m01;
	float tz = m02;
	m00 = tx * a.m00 + ty * a.m10 + tz * a.m20;
	m01 = tx * a.m01 + ty * a.m11 + tz * a.m21;
	m02 = tx * a.m02 + ty * a.m12 + tz * a.m22;
	m03 += tx * a.m03 + ty * a.m13 + tz * a.m23;
	tx = m10;
	ty = m11;
	tz = m12;
	m10 = tx * a.m00 + ty * a.m10 + tz * a.m20;
	m11 = tx * a.m01 + ty * a.m11 + tz * a.m21;
	m12 = tx * a.m02 + ty * a.m12 + tz * a.m22;
	m13 += tx * a.m03 + ty * a.m13 + tz * a.m23;
	tx = m20;
	ty = m21;
	tz = m22;
	m20 = tx * a.m00 + ty * a.m10 + tz * a.m20;
	m21 = tx * a.m01 + ty * a.m11 + tz * a.m21;
	m22 = tx * a.m02 + ty * a.m12 + tz * a.m22;
	m23 += tx * a.m03 + ty * a.m13 + tz * a.m23;
	tx = m30;
	ty = m31;
	tz = m32;
	m30 = tx * a.m00 + ty * a.m10 + tz * a.m20;
	m31 = tx * a.m01 + ty * a.m11 + tz * a.m21;
	m32 = tx * a.m02 + ty * a.m12 + tz * a.m22;
	m33 += tx * a.m03 + ty * a.m13 + tz * a.m23;
}

void Matrix44::setMul( const Matrix44& a, const Matrix34& b ){
	float tx = a.m00;
	float ty = a.m01;
	float tz = a.m02;
	m00 = tx * b.m00 + ty * b.m10 + tz * b.m20;
	m01 = tx * b.m01 + ty * b.m11 + tz * b.m21;
	m02 = tx * b.m02 + ty * b.m12 + tz * b.m22;
	m03 = tx * b.m03 + ty * b.m13 + tz * b.m23 + a.m03;
	tx = a.m10;
	ty = a.m11;
	tz = a.m12;
	m10 = tx * b.m00 + ty * b.m10 + tz * b.m20;
	m11 = tx * b.m01 + ty * b.m11 + tz * b.m21;
	m12 = tx * b.m02 + ty * b.m12 + tz * b.m22;
	m13 = tx * b.m03 + ty * b.m13 + tz * b.m23 + a.m13;
	tx = a.m20;
	ty = a.m21;
	tz = a.m22;
	m20 = tx * b.m00 + ty * b.m10 + tz * b.m20;
	m21 = tx * b.m01 + ty * b.m11 + tz * b.m21;
	m22 = tx * b.m02 + ty * b.m12 + tz * b.m22;
	m23 = tx * b.m03 + ty * b.m13 + tz * b.m23 + a.m23;
	tx = a.m30;
	ty = a.m31;
	tz = a.m32;
	m30 = tx * b.m00 + ty * b.m10 + tz * b.m20;
	m31 = tx * b.m01 + ty * b.m11 + tz * b.m21;
	m32 = tx * b.m02 + ty * b.m12 + tz * b.m22;
	m33 = tx * b.m03 + ty * b.m13 + tz * b.m23 + a.m33;
}

float& Matrix44::operator()( int r, int c ){
	ASSERT( r >= 0 && r < 4 );
	ASSERT( c >= 0 && c < 4 );
	return ( &m00 )[ 4 * r + c ];
}

float Matrix44::operator()( int r, int c ) const {
	ASSERT( r >= 0 && r < 4 );
	ASSERT( c >= 0 && c < 4 );
	return ( &m00 )[ 4 * r + c ];
}

bool Matrix44::operator==( const Matrix44& a ) const {
	for ( int i = 0; i < 4; ++i ){
		for ( int j = 0; j < 4; ++j ){
			if ( ( *this )( i, j ) != a( i, j ) ){
				return false;
			}
		}
	}
	return true;
}

bool Matrix44::operator!=( const Matrix44& a ) const {
	return !( operator==( a ) );
}

} //namespace Math
} //namespace GameLib
