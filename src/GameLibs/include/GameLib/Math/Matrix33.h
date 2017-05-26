#ifndef INCLUDED_GAMELIB_MATH_MATRIX33_H
#define INCLUDED_GAMELIB_MATH_MATRIX33_H

namespace GameLib{
namespace Math{

class Vector3;

class Matrix33{
public:
	Matrix33();

	void setIdentity();
	void setRotationX( float );
	void setRotationY( float );
	void setRotationZ( float );
	void setScaling( float xyz );
	void setScaling( float x, float y, float z );
	void setScaling( const Vector3& );

	void rotateX( float );
	void rotateY( float );
	void rotateZ( float );
	void scale( float xyz );
	void scale( float x, float y, float z );
	void scale( const Vector3& );

	void mul( Vector3* out, const Vector3& in ) const;
	void operator*=( const Matrix33& );
	void setMul( const Matrix33&, const Matrix33& );

	///完全に一致しているかチェック
	bool operator==( const Matrix33& ) const;
	///少しでも違うのかをチェック
	bool operator!=( const Matrix33& ) const;

	float& operator()( int row, int col );
	float operator()( int row, int col ) const;

	void transpose();
	void setTransposed( const Matrix33& );

	void invert();
	void setInverse( const Matrix33& );

	float m00, m01, m02;
	float m10, m11, m12;
	float m20, m21, m22;
};

} //namespace Math
} //namespace GameLib

#endif
