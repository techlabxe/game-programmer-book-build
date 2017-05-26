#include "GameLib/GameLib.h"
#include "GameLib/Math/Vector4.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Functions.h"

namespace GameLib{
namespace Math{

Vector4::Vector4(){
}

Vector4::Vector4( float a ) :
x( a ),
y( a ),
z( a ),
w( a ){
}

Vector4::Vector4( float aX, float aY, float aZ, float aW ) :
x( aX ),
y( aY ),
z( aZ ),
w( aW ){
}

Vector4::Vector4( const Vector4& a ) :
x( a.x ),
y( a.y ),
z( a.z ),
w( a.w ){
}

Vector4::Vector4( const Vector3& a ) :
x( a.x ),
y( a.y ),
z( a.z ),
w( 1.f ){
}

Vector4& Vector4::operator=( const Vector4& a ){
	x = a.x;
	y = a.y;
	z = a.z;
	w = a.w;
	return *this;
}

Vector4& Vector4::operator=( const Vector3& a ){
	x = a.x;
	y = a.y;
	z = a.z;
	w = 1.f;
	return *this;
}

void Vector4::set( float aX, float aY, float aZ, float aW ){
	x = aX;
	y = aY;
	z = aZ;
	w = aW;
}

float& Vector4::operator[]( int i ){
	ASSERT( i >= 0 && i < 4 );
	return ( &x )[ i ];
}

float Vector4::operator[]( int i ) const {
	ASSERT( i >= 0 && i < 4 );
	return ( &x )[ i ];
}

} //namespace Math
} //namespace GameLib

