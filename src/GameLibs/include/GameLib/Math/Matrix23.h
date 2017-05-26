#ifndef INCLUDED_GAMELIB_MATH_MATRIX23_H
#define INCLUDED_GAMELIB_MATH_MATRIX23_H

namespace GameLib{
namespace Math{

class Vector2;
class Matrix22;

class Matrix23{
public:
	Matrix23();

	void setIdentity();
	void setTranslation( float, float );
	void setTranslation( const Vector2& );
	void setRotation( float );
	void setScaling( float xy );
	void setScaling( float x, float y );
	void setScaling( const Vector2& );

	void translate( float, float );
	void translate( const Vector2& );
	void rotate( float );
	void scale( float xy );
	void scale( float x, float y );
	void scale( const Vector2& );

	void mul( Vector2* out, const Vector2& in ) const;
	void operator*=( const Matrix23& );
	void operator*=( const Matrix22& );
	void setMul( const Matrix23&, const Matrix23& );
	void setMul( const Matrix23&, const Matrix22& );

	///完全に一致しているかチェック
	bool operator==( const Matrix23& ) const;
	///少しでも違うのかをチェック
	bool operator!=( const Matrix23& ) const;

	float& operator()( int row, int col );
	float operator()( int row, int col ) const;

	void transpose22();
	void setTransposed22( const Matrix23& );

	void invert();
	void setInverse( const Matrix23& );

	float m00, m01, m02;
	float m10, m11, m12;
};

} //namespace Math
} //namespace GameLib

#endif
