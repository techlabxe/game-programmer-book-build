#include "GameLib/GameLib.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Functions.h"

namespace GameLib{
namespace Math{

Vector3::Vector3(){
}
	
Vector3::Vector3( float a ) : x( a ), y( a ), z( a ){
}

Vector3::Vector3( float aX, float aY, float aZ ) :
x( aX ),
y( aY ),
z( aZ ){
}

Vector3::Vector3( const Vector3& a ) :
x( a.x ),
y( a.y ),
z( a.z ){
}

Vector3& Vector3::operator=( const Vector3& a ){
	x = a.x;
	y = a.y;
	z = a.z;
	return *this;
}

Vector3& Vector3::operator=( float a ){
	x = y = z = a;
	return *this;
}

void Vector3::set( float aX, float aY, float aZ ){
	x = aX;
	y = aY;
	z = aZ;
}

void Vector3::operator+=( const Vector3& a ){
	x += a.x;
	y += a.y;
	z += a.z;
}

void Vector3::operator+=( float a ){
	x += a;
	y += a;
	z += a;
}

void Vector3::operator-=( const Vector3& a ){
	x -= a.x;
	y -= a.y;
	z -= a.z;
}

void Vector3::operator-=( float a ){
	x -= a;
	y -= a;
	z -= a;
}

void Vector3::operator*=( const Vector3& a ){
	x *= a.x;
	y *= a.y;
	z *= a.z;
}

void Vector3::operator*=( float a ){
	x *= a;
	y *= a;
	z *= a;
}

float& Vector3::operator[]( int i ){
	ASSERT( i >= 0 && i < 3 );
	return ( &x )[ i ];
}

float Vector3::operator[]( int i ) const {
	ASSERT( i >= 0 && i < 3 );
	return ( &x )[ i ];
}

void Vector3::setAdd( const Vector3& a, const Vector3& b ){
	x = a.x + b.x;
	y = a.y + b.y;
	z = a.z + b.z;
}

void Vector3::setAdd( const Vector3& a, float b ){
	x = a.x + b;
	y = a.y + b;
	z = a.z + b;
}

void Vector3::setSub( const Vector3& a, const Vector3& b ){
	x = a.x - b.x;
	y = a.y - b.y;
	z = a.z - b.z;
}

void Vector3::setSub( const Vector3& a, float b ){
	x = a.x - b;
	y = a.y - b;
	z = a.z - b;
}

void Vector3::setMul( const Vector3& a, const Vector3& b ){
	x = a.x * b.x;
	y = a.y * b.y;
	z = a.z * b.z;
}

void Vector3::setMul( const Vector3& a, float b ){
	x = a.x * b;
	y = a.y * b;
	z = a.z * b;
}

void Vector3::setNeg( const Vector3& a ){
	x = -a.x;
	y = -a.y;
	z = -a.z;
}

void Vector3::setMadd( const Vector3& a, const Vector3& b, float c ){
	x = a.x + b.x * c;
	y = a.y + b.y * c;
	z = a.z + b.z * c;
}

void Vector3::setMsub( const Vector3& a, const Vector3& b, float c ){
	x = a.x - b.x * c;
	y = a.y - b.y * c;
	z = a.z - b.z * c;
}

void Vector3::madd( const Vector3& a, float b ){
	x += a.x * b;
	y += a.y * b;
	z += a.z * b;
}

void Vector3::msub( const Vector3& a, float b ){
	x -= a.x * b;
	y -= a.y * b;
	z -= a.z * b;
}


float Vector3::dot( const Vector3& a ) const {
	return ( x * a.x ) + ( y * a.y ) + ( z * a.z );
}

void Vector3::setCross( const Vector3& a, const Vector3& b ){
	x = ( a.y * b.z ) - ( a.z * b.y );
	y = ( a.z * b.x ) - ( a.x * b.z );
	z = ( a.x * b.y ) - ( a.y * b.x );
}

void Vector3::normalize(){
	float t = x * x + y * y + z * z;
	ASSERT( t > 0.f );
	t = 1.f / sqrt( t );
	x *= t;
	y *= t;
	z *= t;
}

float Vector3::length() const {
	return sqrt( x * x + y * y + z * z );
}

float Vector3::squareLength() const {
	return x * x + y * y + z * z;
}

void Vector3::min( const Vector3& a ){
	x = Math::min( x, a.x );
	y = Math::min( y, a.y );
	z = Math::min( z, a.z );
}

void Vector3::max( const Vector3& a ){
	x = Math::max( x, a.x );
	y = Math::max( y, a.y );
	z = Math::max( z, a.z );
}

void Vector3::setMin( const Vector3& a, const Vector3& b ){
	x = Math::min( a.x, b.x );
	y = Math::min( a.y, b.y );
	z = Math::min( a.z, b.z );
}

void Vector3::setMax( const Vector3& a, const Vector3& b ){
	x = Math::max( a.x, b.x );
	y = Math::max( a.y, b.y );
	z = Math::max( a.z, b.z );
}

} //namespace Math
} //namespace GameLib

