#include "Library/Matrix44.h"
#include "Library/Matrix34.h"
#include "Library/Vector3.h"
#include "GameLib/GameLibMath.h"
using namespace GameLib;

Matrix44::Matrix44(){
}

void Matrix44::setPerspectiveTransform(
double fieldOfViewY,
double windowWidth,
double windowHeight,
double nearClip,
double farClip,
bool multiplyWToZByHand ){
	double s = 1.0 / GameLib::tan( fieldOfViewY * 0.5 );
	double r = windowHeight / windowWidth;
	double a;
	if ( multiplyWToZByHand ){
		a = 1.0 / ( nearClip - farClip );
	}else{
		a = farClip / ( nearClip - farClip );
	}
	double b = nearClip * a;
	m00 = s * r;
	m11 = s;
	m22 = a;
	m23 = b;
	m32 = -1.0;
	//残り全部0
	m01 = m02 = m03 = 0.0;
	m10 = m12 = m13 = 0.0;
	m20 = m21 = 0.0;
	m30 = m31 = m33 = 0.0;
}

//絶対in!=outなのでバックアップは不要
void Matrix44::multiply( double* out, const Vector3& in ) const {
	out[ 0 ] = m00 * in.x + m01 * in.y + m02 * in.z + m03;
	out[ 1 ] = m10 * in.x + m11 * in.y + m12 * in.z + m13;
	out[ 2 ] = m20 * in.x + m21 * in.y + m22 * in.z + m23;
	out[ 3 ] = m30 * in.x + m31 * in.y + m32 * in.z + m33;
}

void Matrix44::operator*=( const Matrix34& m ){
	setMul( *this, m ); //setMulを流用しよう。処理は同じだ。
}

void Matrix44::setMul( const Matrix44& a, const Matrix34& b ){
	//a == thisの可能性があるのでバックアップは必要
	double tx, ty, tz;
	tx = a.m00; ty = a.m01; tz = a.m02;
	m00 = tx * b.m00 + ty * b.m10 + tz * b.m20;
	m01 = tx * b.m01 + ty * b.m11 + tz * b.m21;
	m02 = tx * b.m02 + ty * b.m12 + tz * b.m22;
	m03 = tx * b.m03 + ty * b.m13 + tz * b.m23 + a.m03;
	tx = a.m10; ty = a.m11; tz = a.m12;
	m10 = tx * b.m00 + ty * b.m10 + tz * b.m20;
	m11 = tx * b.m01 + ty * b.m11 + tz * b.m21;
	m12 = tx * b.m02 + ty * b.m12 + tz * b.m22;
	m13 = tx * b.m03 + ty * b.m13 + tz * b.m23 + a.m13;
	tx = a.m20; ty = a.m21; tz = a.m22;
	m20 = tx * b.m00 + ty * b.m10 + tz * b.m20;
	m21 = tx * b.m01 + ty * b.m11 + tz * b.m21;
	m22 = tx * b.m02 + ty * b.m12 + tz * b.m22;
	m23 = tx * b.m03 + ty * b.m13 + tz * b.m23 + a.m23;
	tx = a.m30; ty = a.m31; tz = a.m32;
	m30 = tx * b.m00 + ty * b.m10 + tz * b.m20;
	m31 = tx * b.m01 + ty * b.m11 + tz * b.m21;
	m32 = tx * b.m02 + ty * b.m12 + tz * b.m22;
	m33 = tx * b.m03 + ty * b.m13 + tz * b.m23 + a.m33;
}
