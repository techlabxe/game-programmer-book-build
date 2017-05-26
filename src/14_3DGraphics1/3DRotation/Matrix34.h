#ifndef INCLUDED_MATRIX34_H
#define INCLUDED_MATRIX34_H

class Vector3;

class Matrix34{
public:
	Matrix34(); //ƒfƒtƒHƒ‹ƒg‚ÅƒRƒ“ƒXƒgƒ‰ƒNƒg
	//ˆÚ“®s—ñ¶¬
	void setTranslation( const Vector3& );
	//Šg‘åk¬s—ñ¶¬
	void setScaling( const Vector3& );
	//‰ñ“]s—ñ¶¬X
	void setRotationX( double radian );
	//‰ñ“]s—ñ¶¬Y
	void setRotationY( double radian );
	//‰ñ“]s—ñ¶¬Z
	void setRotationZ( double radian );
	//ƒxƒNƒ^‚ÆŠ|‚¯Z
	void multiply( Vector3* out, const Vector3& in ) const;
	//s—ñ‚ÆŠ|‚¯Z
	void operator*=( const Matrix34& );
	//ˆÚ“®s—ñ‚ÆæZ
	void translate( const Vector3& );
	//Šg‘åk¬s—ñ‚ÆæZ
	void scale( const Vector3& );
	//X‰ñ“]s—ñ‚ÆæZ
	void rotateX( double radian );
	//Y‰ñ“]s—ñ‚ÆæZ
	void rotateY( double radian );
	//Z‰ñ“]s—ñ‚ÆæZ
	void rotateZ( double radian );
private:
	double m00, m01, m02, m03;
	double m10, m11, m12, m13;
	double m20, m21, m22, m23;
};

#endif
