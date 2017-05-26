#ifndef INCLUDED_MATRIX22_H
#define INCLUDED_MATRIX22_H

class Vector2;

class Matrix22{
public:
	//4要素与えてコンストラクト
	Matrix22( double e00, double e01, double e10, double e11 );
	//ベクタと掛け算
	void multiply( Vector2* out, const Vector2& in ) const;
private:
	double m00, m01, m10, m11;
};

#endif
