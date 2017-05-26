#ifndef INCLUDED_MATRIX44_H
#define INCLUDED_MATRIX44_H

class Vector3;
class Matrix34;

class Matrix44{
public:
	Matrix44(); //デフォルトでコンストラクト
	//透視変換行列生成
	void setPerspectiveTransform( 
		double fieldOfViewY,
		double windowWidth,
		double windowHeight,
		double nearClip,
		double farClip,
		bool multiplyWToZByHand = false ); //自力でZにWをかけるか
	//ベクタと掛け算
	void multiply( double* out, const Vector3& in ) const;
	//行列と掛け算
	void operator*=( const Matrix34& );
private:
	double m00, m01, m02, m03;
	double m10, m11, m12, m13;
	double m20, m21, m22, m23;
	double m30, m31, m32, m33;
};

#endif
