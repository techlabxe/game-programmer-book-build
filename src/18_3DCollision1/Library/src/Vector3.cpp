#include "Library/Vector3.h"

Vector3::Vector3(){}

Vector3::Vector3( double aX, double aY, double aZ ) : 
x( aX ), 
y( aY ),
z( aZ ){
}

Vector3::Vector3( const Vector3& a ) :
x( a.x ),
y( a.y ),
z( a.z ){
}

void Vector3::set( double aX, double aY, double aZ ){
	x = aX;
	y = aY;
	z = aZ;
}

void Vector3::operator+=( const Vector3& a ){
	x += a.x;
	y += a.y;
	z += a.z;
}

void Vector3::operator-=( const Vector3& a ){
	x -= a.x;
	y -= a.y;
	z -= a.z;
}

void Vector3::operator=( const Vector3& a ){
	x = a.x;
	y = a.y;
	z = a.z;
}

void Vector3::operator*=( double a ){
	x *= a;
	y *= a;
	z *= a;
}

void Vector3::setAdd( const Vector3& a, const Vector3& b ){
	x = a.x + b.x;
	y = a.y + b.y;
	z = a.z + b.z;
}

void Vector3::setSub( const Vector3& a, const Vector3& b ){
	x = a.x - b.x;
	y = a.y - b.y;
	z = a.z - b.z;
}

void Vector3::setMul( const Vector3& a, const double b ){
	x = a.x * b;
	y = a.y * b;
	z = a.z * b;
}

double Vector3::squareLength() const {
	return ( x * x ) + ( y * y ) + ( z * z );
}

double Vector3::dot( const Vector3& a ) const {
	return ( x * a.x ) + ( y * a.y ) + ( z * a.z );
}

void Vector3::setCross( const Vector3& a, const Vector3& b ){
	x = ( a.y * b.z ) - ( a.z * b.y );
	y = ( a.z * b.x ) - ( a.x * b.z );
	z = ( a.x * b.y ) - ( a.y * b.x );
}

void Vector3::setMadd( const Vector3& a, double b, const Vector3& c ){
	x = a.x * b + c.x;
	y = a.y * b + c.y;
	z = a.z * b + c.z;
}

