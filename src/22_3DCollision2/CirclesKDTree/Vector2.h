#ifndef INCLUDED_VECTOR2_H
#define INCLUDED_VECTOR2_H

#include <cmath>

class Vector2{
public:
	Vector2(){}
	Vector2( double aX, double aY ) : x( aX ), y( aY ){}
	void set( double aX, double aY ){
		x = aX; y = aY;
	}
	void setMul( const Vector2& a, double b ){
		x = a.x * b;
		y = a.y * b;
	}
	void setSub( const Vector2& a, const Vector2& b ){
		x = a.x - b.x;
		y = a.y - b.y;
	}
	void operator+=( const Vector2& a ){
		x += a.x;
		y += a.y;
	}
	void operator-=( const Vector2& a ){
		x -= a.x;
		y -= a.y;
	}
	void operator*=( double a ){
		x *= a;
		y *= a;
	}
	double squareLength() const {
		return x*x + y*y;
	}
	double length() const {
		return std::sqrt( x*x + y*y );
	}
	void madd( const Vector2& a, double b ){
		x += a.x * b;
		y += a.y * b;
	}
	double x, y;
};

#endif