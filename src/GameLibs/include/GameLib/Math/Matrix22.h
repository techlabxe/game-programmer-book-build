#ifndef INCLUDED_GAMELIB_MATH_MATRIX22_H
#define INCLUDED_GAMELIB_MATH_MATRIX22_H

namespace GameLib{
namespace Math{

class Vector2;

class Matrix22{
public:
	Matrix22();

	void setIdentity();
	void setRotation( float );
	void setScaling( float xy );
	void setScaling( float x, float y );
	void setScaling( const Vector2& );

	void rotate( float );
	void scale( float xy );
	void scale( float x, float y );
	void scale( const Vector2& );

	void mul( Vector2* out, const Vector2& in ) const;
	void operator*=( const Matrix22& );
	void setMul( const Matrix22&, const Matrix22& );

	///完全に一致しているかチェック
	bool operator==( const Matrix22& ) const;
	///少しでも違うのかをチェック
	bool operator!=( const Matrix22& ) const;

	float& operator()( int row, int col );
	float operator()( int row, int col ) const;

	void transpose();
	void setTransposed( const Matrix22& );

	void invert();
	void setInverse( const Matrix22& );

	float m00, m01;
	float m10, m11;
};

} //namespace Math
} //namespace GameLib

#endif
