#ifndef INCLUDED_MODEL_H
#define INCLUDED_MODEL_H

#include "Library/Vector3.h"
class Matrix44;
class Batch;

class Model{
public:
	Model( const Batch* );
	~Model();
	void draw( const Matrix44& projectionView ) const;
	void setPosition( const Vector3& );
	void setAngle( const Vector3& );
	void setScale( const Vector3& );
	const Batch* batch() const;
private:
	Vector3 mPosition;
	Vector3 mAngle;
	Vector3 mScale;
	const Batch* mBatch;
};

#endif
