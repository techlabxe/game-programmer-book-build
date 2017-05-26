#include "GameLib/GameLib.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Matrix33.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Math/Vector3.h"

namespace GameLib{
namespace Math{

Matrix34::Matrix34(){
}

void Matrix34::setIdentity(){
	m00 = m11 = m22 = 1.f;
	m01 = m02 = m03 = 0.f;
	m10 = m12 = m13 = 0.f;
	m20 = m21 = m23 = 0.f;
}

void Matrix34::setTranslation( float x, float y, float z ){
	m00 = m11 = m22 = 1.f;
	m03 = x;
	m13 = y;
	m23 = z;
	m01 = m02 = m10 = m12 = m20 = m21 = 0.f;
}

void Matrix34::setTranslation( const Vector3& a ){
	m00 = m11 = m22 = 1.f;
	m03 = a.x;
	m13 = a.y;
	m23 = a.z;
	m01 = m02 = m10 = m12 = m20 = m21 = 0.f;
}

void Matrix34::setRotationX( float r ){
	float c = cos( r );
	float s = sin( r );
	m11 = m22 = c;
	m12 = -s;
	m21 = s;
	m00 = 1.0;
	m01 = m02 = m10 = m20 = 0.0;
	m03 = m13 = m23 = 0.f;
}

void Matrix34::setRotationY( float r ){
	float c = cos( r );
	float s = sin( r );
	m00 = m22 = c;
	m20 = -s;
	m02 = s;
	m11 = 1.0;
	m01 = m10 = m12 = m21 = 0.0;
	m03 = m13 = m23 = 0.f;
}

void Matrix34::setRotationZ( float r ){
	float c = cos( r );
	float s = sin( r );
	m00 = m11 = c;
	m01 = -s;
	m10 = s;
	m22 = 1.0;
	m02 = m12 = m20 = m21 = 0.0;
	m03 = m13 = m23 = 0.f;
}

void Matrix34::setScaling( float xyz ){
	m00 = m11 = m22 = xyz;
	m01 = m02 = m03 = 0.f;
	m10 = m12 = m13 = 0.f;
	m20 = m21 = m23 = 0.f;
}

void Matrix34::setScaling( float x, float y, float z ){
	m00 = x;
	m11 = y;
	m22 = z;
	m01 = m02 = m03 = 0.f;
	m10 = m12 = m13 = 0.f;
	m20 = m21 = m23 = 0.f;
}

void Matrix34::setScaling( const Vector3& a ){
	m00 = a.x;
	m11 = a.y;
	m22 = a.z;
	m01 = m02 = m03 = 0.f;
	m10 = m12 = m13 = 0.f;
	m20 = m21 = m23 = 0.f;
}

/*
3軸回転ビュー行列の作りかた。

回転部分は3x3行列である。よって、数が9個ある。
式が9本あればこれを求められるわけだ。

さて、その式をどこから作るかだ。
あるベクタsがs'に移るとわかっているとしよう。
求めたい回転行列をAとすれば、

s'=As

である。これをばらせば、

s'x = a00*sx+a01*sy+a02*sz
s'y = a10*sx+a11*sy+a12*sz
s'z = a20*sx+a21*sy+a22*sz

と式が3本できる。s以外にtとrについても、
t'=At
r'=Ar
とわかっていれば、さらに式が6本増えるので、
9本の式が立つ。

ここで、計算を単純化するために、
s=(1,0,0)
t=(0,1,0)
r=(0,0,1)
としてみよう。上の式はアホみたいに簡単になって、

s'x = a00
s'y = a10
s'z = a20

となる。t,rについても同じようなことになり、結局Aは
    | s'x t'x r'x |
A = | s'y t'y r'y |
    | s'z t'z r'z |

と書けてしまうわけである。あとは、
s'、t'、r'がわかればいい。

今zプラス方向は注視点から視点へ向かうベクタで、
r' = ( p - t ) / | p - t |
となる。長さを1にしていることに注意しよう。

そしてもう一つ、上がどちらかがuが与えられることでわかっている。
このままY軸とすることはできないが、
uとr'の外積を取るとX軸が出てくる。これがs'だ。
そして、s'とr'の外積から改めてY軸を出す。これがt'となる。
下のコードにおいて、x,y,zはそれぞれs',t',r'である。

回転行列さえできてしまえば、後は移動分を計算するだけだ。
V=RTで、Rは今作った。Tはpが原点に来るような移動行列である。
下のコードでは行列の乗算を丸々やる計算量を避けるために、
RTの掛け算の必要な部分だけをその場で行って、m03,m13,m23を計算している。
*/
void Matrix34::setViewTransform(
const Vector3& p, //視点
const Vector3& t, //注視点
const Vector3& u ){ //上を表すベクタ
	Vector3 x, y, z;
	z.setSub( p, t ); //注視点からカメラ位置へのベクタをZ軸に
	z.normalize();
	x.setCross( u, z ); //上ベクタとZ軸の外積をX軸に。上ベクタが上向きなら左向きがX軸プラス。
	x.normalize();
	y.setCross( z, x ); //Z,Xの外積がY

	//直交基底を3本行方向で入れれば出来上がり。
	m00 = x.x; m01 = x.y; m02 = x.z;
	m10 = y.x; m11 = y.y; m12 = y.z;
	m20 = z.x; m21 = z.y; m22 = z.z;
	//移動分は今作った回転行列にカメラ座標を乗算してマイナスにすればいい。
	m03 = -( m00 * p.x + m01 * p.y + m02 * p.z );
	m13 = -( m10 * p.x + m11 * p.y + m12 * p.z );
	m23 = -( m20 * p.x + m21 * p.y + m22 * p.z );
}

void Matrix34::rotateX( float r ){
	float c = cos( r );
	float s = sin( r );
	float t;
	t = c * m01 + s * m02;
	m02 = -s * m01 + c * m02;
	m01 = t;
	t = c * m11 + s * m12;
	m12 = -s * m11 + c * m12;
	m11 = t;
	t = c * m21 + s * m22;
	m22 = -s * m21 + c * m22;
	m21 = t;
}

void Matrix34::rotateY( float r ){
	float c = cos( r );
	float s = sin( r );
	float t;
	t = c * m00 - s * m02;
	m02 = s * m00 + c * m02;
	m00 = t;
	t = c * m10 - s * m12;
	m12 = s * m10 + c * m12;
	m10 = t;
	t = c * m20 - s * m22;
	m22 = s * m20 + c * m22;
	m20 = t;
}

void Matrix34::rotateZ( float r ){
	float c = cos( r );
	float s = sin( r );
	float t;
	t = c * m00 + s * m01;
	m01 = -s * m00 + c * m01;
	m00 = t;
	t = c * m10 + s * m11;
	m11 = -s * m10 + c * m11;
	m10 = t;
	t = c * m20 + s * m21;
	m21 = -s * m20 + c * m21;
	m20 = t;
}

void Matrix34::scale( float xyz ){
	m00 *= xyz;
	m01 *= xyz;
	m02 *= xyz;
	m10 *= xyz;
	m11 *= xyz;
	m12 *= xyz;
	m20 *= xyz;
	m21 *= xyz;
	m22 *= xyz;
}

void Matrix34::scale( float x, float y, float z ){
	m00 *= x;
	m01 *= y;
	m02 *= z;
	m10 *= x;
	m11 *= y;
	m12 *= z;
	m20 *= x;
	m21 *= y;
	m22 *= z;
}

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

void Matrix34::translate( float x, float y, float z ){
	m03 += m00 * x + m01 * y + m02 * z;
	m13 += m10 * x + m11 * y + m12 * z;
	m23 += m20 * x + m21 * y + m22 * z;
}

void Matrix34::translate( const Vector3& a ){
	m03 += m00 * a.x + m01 * a.y + m02 * a.z;
	m13 += m10 * a.x + m11 * a.y + m12 * a.z;
	m23 += m20 * a.x + m21 * a.y + m22 * a.z;
}


void Matrix34::mul( Vector3* out, const Vector3& in ) const {
	float tx = in.x;
	float ty = in.y;
	out->x = m00 * tx + m01 * ty + m02 * in.z + m03;
	out->y = m10 * tx + m11 * ty + m12 * in.z + m13;
	out->z = m20 * tx + m21 * ty + m22 * in.z + m23;
}

void Matrix34::mul33( Vector3* out, const Vector3& in ) const {
	float tx = in.x;
	float ty = in.y;
	out->x = m00 * tx + m01 * ty + m02 * in.z;
	out->y = m10 * tx + m11 * ty + m12 * in.z;
	out->z = m20 * tx + m21 * ty + m22 * in.z;
}

void Matrix34::operator*=( const Matrix34& a ){
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
}

void Matrix34::operator*=( const Matrix33& a ){
	float tx = m00;
	float ty = m01;
	m00 = tx * a.m00 + ty * a.m10 + m02 * a.m20;
	m01 = tx * a.m01 + ty * a.m11 + m02 * a.m21;
	m02 = tx * a.m02 + ty * a.m12 + m02 * a.m22;
	tx = m10;
	ty = m11;
	m10 = tx * a.m00 + ty * a.m10 + m12 * a.m20;
	m11 = tx * a.m01 + ty * a.m11 + m12 * a.m21;
	m12 = tx * a.m02 + ty * a.m12 + m12 * a.m22;
	tx = m20;
	ty = m21;
	m20 = tx * a.m00 + ty * a.m10 + m22 * a.m20;
	m21 = tx * a.m01 + ty * a.m11 + m22 * a.m21;
	m22 = tx * a.m02 + ty * a.m12 + m22 * a.m22;
}

void Matrix34::setMul( const Matrix34& a, const Matrix34& b ){
	//a==*this, b==*this両方を考えないといけない
	float t00, t01, t02, t03;
	t00 = a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20;
	t01 = a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21;
	t02 = a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22;
	t03 = a.m00 * b.m03 + a.m01 * b.m13 + a.m02 * b.m23 + a.m03;

	float t10, t11, t12, t13;
	t10 = a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20;
	t11 = a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21;
	t12 = a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22;
	t13 = a.m10 * b.m03 + a.m11 * b.m13 + a.m12 * b.m23 + a.m13;

	float t20, t21, t22;
	t20 = a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20;
	t21 = a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21;
	t22 = a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22;
	m23 = a.m20 * b.m03 + a.m21 * b.m13 + a.m22 * b.m23 + a.m23;

	m00 = t00; m01 = t01; m02 = t02; m03 = t03;
	m10 = t10; m11 = t11; m12 = t12; m13 = t13;
	m20 = t20; m21 = t21; m22 = t22;
}

void Matrix34::setMul( const Matrix34& a, const Matrix33& b ){
	float tx = a.m00;
	float ty = a.m01;
	m00 = tx * b.m00 + ty * b.m10 + a.m02 * b.m20;
	m01 = tx * b.m01 + ty * b.m11 + a.m02 * b.m21;
	m02 = tx * b.m02 + ty * b.m12 + a.m02 * b.m22;
	m03 = a.m03;
	tx = a.m10;
	ty = a.m11;
	m10 = tx * b.m00 + ty * b.m10 + a.m12 * b.m20;
	m11 = tx * b.m01 + ty * b.m11 + a.m12 * b.m21;
	m12 = tx * b.m02 + ty * b.m12 + a.m12 * b.m22;
	m13 = a.m13;
	tx = a.m20;
	ty = a.m21;
	m20 = tx * b.m00 + ty * b.m10 + a.m22 * b.m20;
	m21 = tx * b.m01 + ty * b.m11 + a.m22 * b.m21;
	m22 = tx * b.m02 + ty * b.m12 + a.m22 * b.m22;
	m23 = a.m23;
}

float& Matrix34::operator()( int r, int c ){
	ASSERT( r >= 0 && r < 3 );
	ASSERT( c >= 0 && c < 4 );
	return ( &m00 )[ 4 * r + c ];
}

float Matrix34::operator()( int r, int c ) const {
	ASSERT( r >= 0 && r < 3 );
	ASSERT( c >= 0 && c < 4 );
	return ( &m00 )[ 4 * r + c ];
}

void Matrix34::transpose33(){
	float t;
	t = m01; m01 = m10; m10 = t;
	t = m02; m02 = m20; m20 = t;
	t = m12; m12 = m21; m21 = t;
	m03 = m13 = m23 = 0.f;
}

void Matrix34::setTransposed33( const Matrix34& a ){
	m00 = a.m00;
	m01 = a.m10;
	m02 = a.m20;
	m10 = a.m01;
	m11 = a.m11;
	m12 = a.m12;
	m20 = a.m21;
	m21 = a.m21;
	m22 = a.m22;
	m03 = m13 = m23 = 0.f;
}

//クラーメル法での逆行列
/*
33のやり方に、23に書いた考え方を適用すると、あっさり解ける。
Matrix33.cppとMatrix23.cppを見よう。
*/
void Matrix34::setInverse( const Matrix34& a ){
	//まず3x3部分の逆行列を作る
//--3x3からこぴぺ
	//二項積をまとめて作る(添え字が小さい順に並べてみたので間違いが少ないかと思う)
	float m0011 = a.m00 * a.m11;
	float m0012 = a.m00 * a.m12;
	float m0021 = a.m00 * a.m21;
	float m0022 = a.m00 * a.m22;

	float m0110 = a.m01 * a.m10;
	float m0112 = a.m01 * a.m12;
	float m0120 = a.m01 * a.m20;
	float m0122 = a.m01 * a.m22;

	float m0210 = a.m02 * a.m10;
	float m0211 = a.m02 * a.m11;
	float m0220 = a.m02 * a.m20;
	float m0221 = a.m02 * a.m21;

	float m1021 = a.m10 * a.m21;
	float m1022 = a.m10 * a.m22;

	float m1120 = a.m11 * a.m20;
	float m1122 = a.m11 * a.m22;

	float m1220 = a.m12 * a.m20;
	float m1221 = a.m12 * a.m21; //18個

	//2項積の差を三つ。行列式用だが、後で使いまわせる。
	float m1122_m1221 = m1122 - m1221;
	float m1220_m1022 = m1220 - m1022; //マイナス
	float m1021_m1120 = m1021 - m1120;

	//行列式
	//00*( 11*22 - 12*21 ) - 01*( 10*22 - 12*20 ) + 02*( 10*21 - 11*20 )
	float delta = a.m00*( m1122_m1221 ) + a.m01*( m1220_m1022 ) + a.m02*( m1021_m1120 );
	float rcpDelta = 1.f / delta; //0割する可能性はあるが、ここでははじかない。

	//要素を埋める。転置に注意！左辺は添え字がひっくり返っている！
	m00 = ( m1122_m1221 ) * rcpDelta;
	m10 = ( m1220_m1022 ) * rcpDelta; //マイナス
	m20 = ( m1021_m1120 ) * rcpDelta;
	m01 = ( m0221-m0122 ) * rcpDelta; //マイナス
	m11 = ( m0022-m0220 ) * rcpDelta;
	m21 = ( m0120-m0021 ) * rcpDelta; //マイナス
	m02 = ( m0112-m0211 ) * rcpDelta;
	m12 = ( m0210-m0012 ) * rcpDelta; //マイナス
	m22 = ( m0011-m0110 ) * rcpDelta;
//----ここまでこぴぺ
	float t03 = a.m03; //バックアップ
	float t13 = a.m13;
	m03 = -( m00 * t03 + m01 * t13 + m02 * a.m23 );
	m13 = -( m10 * t03 + m11 * t13 + m12 * a.m23 );
	m23 = -( m20 * t03 + m21 * t13 + m22 * a.m23 );
}

void Matrix34::invert(){
	setInverse( *this ); //setInverse()が自分でも大丈夫な作りなのでそのまま渡す。
}

bool Matrix34::operator==( const Matrix34& a ) const {
	for ( int i = 0; i < 3; ++i ){
		for ( int j = 0; j < 4; ++j ){
			if ( ( *this )( i, j ) != a( i, j ) ){
				return false;
			}
		}
	}
	return true;
}

bool Matrix34::operator!=( const Matrix34& a ) const {
	return !( operator==( a ) );
}




} //namespace Math
} //namespace GameLib
