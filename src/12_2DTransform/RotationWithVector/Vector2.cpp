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
