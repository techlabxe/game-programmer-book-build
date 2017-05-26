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
	//回転行列生成X
	void setRotationX( double radian );
	//回転行列生成Y
	void setRotationY( double radian );
	//回転行列生成Z
	void setRotationZ( double radian );
	//ビュー行列生成
	void setViewTransform( const Vector3& position, const Vector3& target );
	//ベクタと掛け算
	void multiply( Vector3* out, const Vector3& in ) const;
	//行列と掛け算
	void operator*=( const Matrix34& );
	//移動行列と乗算
	void translate( const Vector3& );
	//拡大縮小行列と乗算
	void scale( const Vector3& );
	//X回転行列と乗算
	void rotateX( double radian );
	//Y回転行列と乗算
	void rotateY( double radian );
	//Z回転行列と乗算
	void rotateZ( double radian );

	double m00, m01, m02, m03;
	double m10, m11, m12, m13;
	double m20, m21, m22, m23;
};

#endif
