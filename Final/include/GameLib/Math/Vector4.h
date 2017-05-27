#ifndef INCLUDED_GAMELIB_MATH_VECTOR4_H
#define INCLUDED_GAMELIB_MATH_VECTOR4_H

namespace GameLib{
namespace Math{

class Vector3;

///3次元同次座標として使うべきクラス。そのため演算はほとんど用意しない。
class Vector4{
public:
	Vector4();
	explicit Vector4( float );
	Vector4( float, float, float, float = 1.f );
	Vector4( const Vector4& );
	explicit Vector4( const Vector3& );
	Vector4& operator=( const Vector4& );
	Vector4& operator=( const Vector3& );
	void set( float, float, float, float = 1.f );

	float& operator[]( int );
	float operator[]( int ) const;

	float x, y, z, w;
};

} //namespace Math
} //namespace GameLib

#endif
