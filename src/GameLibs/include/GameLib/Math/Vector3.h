#ifndef INCLUDED_GAMELIB_MATH_VECTOR3_H
#define INCLUDED_GAMELIB_MATH_VECTOR3_H

namespace GameLib{
namespace Math{

class Vector3{
public:
	Vector3();
	explicit Vector3( float ); //x=y=z=arg.
	Vector3( float, float, float );
	Vector3( const Vector3& );
	Vector3& operator=( const Vector3& );
	Vector3& operator=( float ); //x=y=z=arg.
	void set( float, float, float );

	void operator+=( const Vector3& );
	void operator+=( float );
	void operator-=( const Vector3& );
	void operator-=( float );
	void operator*=( const Vector3& );
	void operator*=( float );
	float& operator[]( int );
	float operator[]( int ) const;

	void setAdd( const Vector3&, const Vector3& );
	void setAdd( const Vector3&, float );
	void setSub( const Vector3&, const Vector3& );
	void setSub( const Vector3&, float );
	void setMul( const Vector3&, const Vector3& );
	void setMul( const Vector3&, float );
	///ƒ}ƒCƒiƒX‚É‚µ‚ÄŠi”[
	void setNeg( const Vector3& );

	/// this = a + b * c
	void setMadd( const Vector3& a, const Vector3& b, float c );
	/// this = a - b * c
	void setMsub( const Vector3& a, const Vector3& b, float c );
	/// this += a * b
	void madd( const Vector3& a, float b );
	/// this -= a * b
	void msub( const Vector3& a, float b );

	///dot product
	float dot( const Vector3& ) const;
	///this = crossProduct( a, b )
	void setCross( const Vector3& a, const Vector3& b );
	///normalize
	void normalize();
	float length() const;
	float squareLength() const;

	void min( const Vector3& );
	void max( const Vector3& );
	void setMin( const Vector3&, const Vector3& );
	void setMax( const Vector3&, const Vector3& );

	float x, y, z;
};

} //namespace Math
} //namespace GameLib

#endif