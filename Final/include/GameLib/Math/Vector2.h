#ifndef INCLUDED_GAMELIB_MATH_VECTOR2_H
#define INCLUDED_GAMELIB_MATH_VECTOR2_H

namespace GameLib{
namespace Math{

class Vector2{
public:
	Vector2();
	explicit Vector2( float ); //x=y=arg.
	Vector2( float, float );
	Vector2( const Vector2& );
	Vector2& operator=( const Vector2& );
	Vector2& operator=( float ); //x=y=arg.
	void set( float, float );

	void operator+=( const Vector2& );
	void operator+=( float );
	void operator-=( const Vector2& );
	void operator-=( float );
	void operator*=( const Vector2& );
	void operator*=( float );
	float& operator[]( int );
	float operator[]( int ) const;

	void setAdd( const Vector2&, const Vector2& );
	void setAdd( const Vector2&, float );
	void setSub( const Vector2&, const Vector2& );
	void setSub( const Vector2&, float );
	void setMul( const Vector2&, const Vector2& );
	void setMul( const Vector2&, float );
	///ƒ}ƒCƒiƒX‚É‚µ‚ÄŠi”[
	void setNeg( const Vector2& );

	/// this = a + b * c
	void setMadd( const Vector2& a, const Vector2& b, float c );
	/// this = a - b * c
	void setMsub( const Vector2& a, const Vector2& b, float c );
	/// this += a * b
	void madd( const Vector2& a, float b );
	/// this -= a * b
	void msub( const Vector2& a, float b );
	/// normalize
	void normalize();
	float length() const;
	float squareLength() const;
	float dot( const Vector2& ) const;

	void min( const Vector2& );
	void max( const Vector2& );
	void setMin( const Vector2&, const Vector2& );
	void setMax( const Vector2&, const Vector2& );

	float x, y;
};

} //namespace Math
} //namespace GameLib

#endif