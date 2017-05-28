#include "Matrix44.h"
#include "Matrix34.h"
#include "Vector3.h"
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
	double tx, ty, tz;
	tx = m00; ty = m01; tz = m02;
	m00 = tx * m.m00 + ty * m.m10 + tz * m.m20;
	m01 = tx * m.m01 + ty * m.m11 + tz * m.m21;
	m02 = tx * m.m02 + ty * m.m12 + tz * m.m22;
	m03 = tx * m.m03 + ty * m.m13 + tz * m.m23 + m03;
	tx = m10; ty = m11; tz = m12;
	m10 = tx * m.m00 + ty * m.m10 + tz * m.m20;
	m11 = tx * m.m01 + ty * m.m11 + tz * m.m21;
	m12 = tx * m.m02 + ty * m.m12 + tz * m.m22;
	m13 = tx * m.m03 + ty * m.m13 + tz * m.m23 + m13;
	tx = m20; ty = m21; tz = m22;
	m20 = tx * m.m00 + ty * m.m10 + tz * m.m20;
	m21 = tx * m.m01 + ty * m.m11 + tz * m.m21;
	m22 = tx * m.m02 + ty * m.m12 + tz * m.m22;
	m23 = tx * m.m03 + ty * m.m13 + tz * m.m23 + m23;
	tx = m30; ty = m31; tz = m32;
	m30 = tx * m.m00 + ty * m.m10 + tz * m.m20;
	m31 = tx * m.m01 + ty * m.m11 + tz * m.m21;
	m32 = tx * m.m02 + ty * m.m12 + tz * m.m22;
	m33 = tx * m.m03 + ty * m.m13 + tz * m.m23 + m33;
}
