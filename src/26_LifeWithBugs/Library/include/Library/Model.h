#ifndef INCLUDED_MODEL_H
#define INCLUDED_MODEL_H

#include "GameLib/Math/Vector3.h"
namespace GameLib{
	namespace Math{
		class Matrix44;
	}
}
using namespace GameLib;
using namespace GameLib::Math;
class Batch;

class Model{
public:
	Model( ConstSharedPtr< Batch > );
	~Model();
	void draw() const;
	void setPosition( const Vector3& );
	void setAngle( const Vector3& );
	void setScale( const Vector3& );
	ConstSharedPtr< Batch > getBatch() const;
	const Vector3* getPosition() const;
	const Vector3* getAngle() const;
private:
	Vector3 mPosition;
	Vector3 mAngle;
	Vector3 mScale;
	ConstSharedPtr< Batch > mBatch;
};

#endif
