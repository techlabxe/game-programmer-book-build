#ifndef INCLUDED_GAMELIB_MATH_MATRIX34_H
#define INCLUDED_GAMELIB_MATH_MATRIX34_H

namespace GameLib{
namespace Math{

class Vector3;
class Matrix33;

class Matrix34{
public:
	Matrix34();

	void setIdentity();
	void setTranslation( float, float, float );
	void setTranslation( const Vector3& );
	void setRotationX( float );
	void setRotationY( float );
	void setRotationZ( float );
	void setScaling( float xyz );
	void setScaling( float x, float y, float z );
	void setScaling( const Vector3& );
	void setViewTransform( 
		const Vector3& eyePosition,
		const Vector3& eyeTarget,
		const Vector3& upVector );

	void translate( float, float, float );
	void translate( const Vector3& );
	void rotateX( float );
	void rotateY( float );
	void rotateZ( float );
	void scale( float xyz );
	void scale( float x, float y, float z );
	void scale( const Vector3& );

	void mul( Vector3* out, const Vector3& in ) const;
	void mul33( Vector3* out, const Vector3& in ) const; //移動なし
 	void operator*=( const Matrix34& );
	void operator*=( const Matrix33& );
	void setMul( const Matrix34&, const Matrix34& );
	void setMul( const Matrix34&, const Matrix33& );

	///完全に一致しているかチェック
	bool operator==( const Matrix34& ) const;
	///少しでも違うのかをチェック
	bool operator!=( const Matrix34& ) const;

	float& operator()( int row, int col );
	float operator()( int row, int col ) const;

	void transpose33();
	void setTransposed33( const Matrix34& );

	void invert(); //逆行列
	void setInverse( const Matrix34& ); //逆行列

	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
};

} //namespace Math
} //namespace GameLib

#endif
