#ifndef INCLUDED_MODEL_H
#define INCLUDED_MODEL_H

#include "Library/Vector3.h"
class Matrix44;
class Batch;

class Model{
public:
	Model( const Batch* );
	~Model();
	void draw( 
		const Matrix44& projectionViewMatrix,
		const Vector3& lightVector,
		const Vector3& lightColor,
		const Vector3& ambient ) const;
	void setPosition( const Vector3& );
	void setAngle( const Vector3& );
	void setScale( const Vector3& );
	const Batch* batch() const;
	const Vector3* position() const;
	const Vector3* angle() const;
	const Vector3* scale() const;
	void setColor( const Vector3& );
private:
	Vector3 mPosition;
	Vector3 mAngle;
	Vector3 mScale;
	Vector3 mColor;
	const Batch* mBatch;
};

#endif
