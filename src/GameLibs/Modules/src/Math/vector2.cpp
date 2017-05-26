#include "GameLib/GameLib.h"
#include "GameLib/Math/Vector2.h"
#include "GameLib/Math/Functions.h"

namespace GameLib{
namespace Math{

Vector2::Vector2(){
}
	
Vector2::Vector2( float a ) : x( a ), y( a ){
}

Vector2::Vector2( float aX, float aY ) :
x( aX ),
y( aY ){
}

Vector2::Vector2( const Vector2& a ) :
x( a.x ),
y( a.y ){
}

Vector2& Vector2::operator=( const Vector2& a ){
	x = a.x;
	y = a.y;
	return *this;
}

Vector2& Vector2::operator=( float a ){
	x = y = a;
	return *this;
}

void Vector2::set( float aX, float aY ){
	x = aX;
	y = aY;
}

void Vector2::operator+=( const Vector2& a ){
	x += a.x;
	y += a.y;
}

void Vector2::operator+=( float a ){
	x += a;
	y += a;
}

void Vector2::operator-=( const Vector2& a ){
	x -= a.x;
	y -= a.y;
}

void Vector2::operator-=( float a ){
	x -= a;
	y -= a;
}

void Vector2::operator*=( const Vector2& a ){
	x *= a.x;
	y *= a.y;
}

void Vector2::operator*=( float a ){
	x *= a;
	y *= a;
}

float& Vector2::operator[]( int i ){
	STRONG_ASSERT( i >= 0 && i < 2 );
	return ( &x )[ i ];
}

float Vector2::operator[]( int i ) const {
	STRONG_ASSERT( i >= 0 && i < 2 );
	return ( &x )[ i ];
}

void Vector2::setAdd( const Vector2& a, const Vector2& b ){
	x = a.x + b.x;
	y = a.y + b.y;
}

void Vector2::setAdd( const Vector2& a, float b ){
	x = a.x + b;
	y = a.y + b;
}

void Vector2::setSub( const Vector2& a, const Vector2& b ){
	x = a.x - b.x;
	y = a.y - b.y;
}

void Vector2::setSub( const Vector2& a, float b ){
	x = a.x - b;
	y = a.y - b;
}

void Vector2::setMul( const Vector2& a, const Vector2& b ){
	x = a.x * b.x;
	y = a.y * b.y;
}

void Vector2::setMul( const Vector2& a, float b ){
	x = a.x * b;
	y = a.y * b;
}

void Vector2::setNeg( const Vector2& a ){
	x = -a.x;
	y = -a.y;
}

void Vector2::setMadd( const Vector2& a, const Vector2& b, float c ){
	x = a.x + b.x * c;
	y = a.y + b.y * c;
}

void Vector2::setMsub( const Vector2& a, const Vector2& b, float c ){
	x = a.x - b.x * c;
	y = a.y - b.y * c;
}

void Vector2::madd( const Vector2& a, float b ){
	x += a.x * b;
	y += a.y * b;
}

void Vector2::msub( const Vector2& a, float b ){
	x -= a.x * b;
	y -= a.y * b;
}

void Vector2::normalize(){
	float t = x * x + y * y;
	ASSERT( t > 0.f );
	t = 1.f / sqrt( t );
	x *= t;
	y *= t;
}

float Vector2::length() const {
	return sqrt( x * x + y * y );
}

float Vector2::squareLength() const {
	return x * x + y * y;
}

float Vector2::dot( const Vector2& a ) const {
	return ( x * a.x ) + ( y * a.y );
}

void Vector2::min( const Vector2& a ){
	x = Math::min( x, a.x );
	y = Math::min( y, a.y );
}

void Vector2::max( const Vector2& a ){
	x = Math::max( x, a.x );
	y = Math::max( y, a.y );
}

void Vector2::setMin( const Vector2& a, const Vector2& b ){
	x = Math::min( a.x, b.x );
	y = Math::min( a.y, b.y );
}

void Vector2::setMax( const Vector2& a, const Vector2& b ){
	x = Math::max( a.x, b.x );
	y = Math::max( a.y, b.y );
}


} //namespace Math
} //namespace GameLib

