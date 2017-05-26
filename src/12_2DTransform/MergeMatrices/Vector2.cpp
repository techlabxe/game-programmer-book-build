#include "Vector2.h"

Vector2::Vector2(){}

Vector2::Vector2( int aX, int aY ){
	x = static_cast< double >( aX );
	y = static_cast< double >( aY );
}

Vector2::Vector2( double aX, double aY ) : 
x( aX ), 
y( aY ){
}

Vector2::Vector2( const Vector2& a ) :
x( a.x ),
y( a.y ){
}

void Vector2::operator+=( const Vector2& a ){
	x += a.x;
	y += a.y;
}

void Vector2::operator-=( const Vector2& a ){
	x -= a.x;
	y -= a.y;
}

void Vector2::operator=( const Vector2& a ){
	x = a.x;
	y = a.y;
}

void Vector2::setAdd( const Vector2& a, const Vector2& b ){
	x = a.x + b.x;
	y = a.y + b.y;
}

void Vector2::setSub( const Vector2& a, const Vector2& b ){
	x = a.x - b.x;
	y = a.y - b.y;
}

void Vector2::setMul( const Vector2& a, const Vector2& b ){
	x = a.x * b.x;
	y = a.y * b.y;
}

void Vector2::setInterporation(
const Vector2& a,
const Vector2& ab,
const Vector2& ac,
double u,
double v ){
	x = a.x + ab.x * u + ac.x * v;
	y = a.y + ab.y * u + ac.y * v;
}
