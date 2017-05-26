#include "Matrix34.h"
#include "Vector3.h"
#include "GameLib/Math.h"
using namespace GameLib;

Matrix34::Matrix34(){
}

void Matrix34::setTranslation( const Vector3& a ){
	m00 = m11 = m22 = 1.0;
	m01 = m10 = m20 = m02 = m12 = m21 = 0.0;
	m03 = a.x;
	m13 = a.y;
	m23 = a.z;
}

void Matrix34::setScaling( const Vector3& a ){
	m00 = a.x;
	m11 = a.y;
	m22 = a.z;
	m01 = m10 = m02 = m20 = m12 = m21 = 0.0;
	m03 = m13 = m23 = 0.0;
}

void Matrix34::operator*=( const Matrix34& m ){
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
}

void Matrix34::multiply( Vector3* out, const Vector3& in ) const {
	//outとinが同じかもしれないので、バックアップ
	double tx = in.x; 
	double ty = in.y; 
	out->x = m00 * tx + m01 * ty + m02 * in.z + m03;
	out->y = m10 * tx + m11 * ty + m12 * in.z + m13;
	out->z = m20 * tx + m21 * ty + m22 * in.z + m23;
}

//無駄なく移動行列を乗算。2Dの時のを参考にして自力で考えよう。
void Matrix34::translate( const Vector3& a ){
	m03 += m00 * a.x + m01 * a.y + m02 * a.z;
	m13 += m10 * a.x + m11 * a.y + m12 * a.z;
	m23 += m20 * a.x + m21 * a.y + m22 * a.z;
}

//無駄なく拡大縮小行列を乗算。2Dの時のを参考にして自力で考えよう。
void Matrix34::scale( const Vector3& a ){
	m00 *= a.x;
	m01 *= a.y;
	m02 *= a.z;
	m10 *= a.x;
	m11 *= a.y;
	m12 *= a.z;
	m20 *= a.x;
	m21 *= a.y;
	m22 *= a.z;
}
