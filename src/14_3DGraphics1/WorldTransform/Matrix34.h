#ifndef INCLUDED_MATRIX34_H
#define INCLUDED_MATRIX34_H

class Vector3;

class Matrix34{
public:
	Matrix34(); //デフォルトでコンストラクト
	//移動行列生成
	void setTranslation( const Vector3& );
	//拡大縮小行列生成
	void setScaling( const Vector3& );
	//ベクタと掛け算
	void multiply( Vector3* out, const Vector3& in ) const;
	//行列と掛け算
	void operator*=( const Matrix34& );
	//移動行列と乗算
	void translate( const Vector3& );
	//拡大縮小行列と乗算
	void scale( const Vector3& );
private:
	double m00, m01, m02, m03;
	double m10, m11, m12, m13;
	double m20, m21, m22, m23;
};

#endif
