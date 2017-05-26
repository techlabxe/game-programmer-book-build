#ifndef INCLUDED_MATRIX23_H
#define INCLUDED_MATRIX23_H

class Vector2;

class Matrix23{
public:
	Matrix23(); //デフォルトでコンストラクト
	//移動行列生成
	void setTranslation( const Vector2& );
	//回転行列生成
	void setRotation( double radian );
	//拡大縮小行列生成
	void setScaling( const Vector2& );
	//ベクタと掛け算
	void multiply( Vector2* out, const Vector2& in ) const;
	//行列と掛け算
	void operator*=( const Matrix23& );
private:
	double m00, m01, m02;
	double m10, m11, m12;
};

#endif
