#ifndef INCLUDED_STAGE_H
#define INCLUDED_STAGE_H

#include "Sphere.h"
#include "Triangle.h"

class Matrix44;
class GraphicsDatabase;
class Model;

class Stage{
public:
	Stage();
	~Stage();
	void draw( const Matrix44& perspectiveViewMatrix ) const;
	void restrictMove( Vector3* moveVectorInOut, const Vector3& position ) const;
private:
	GraphicsDatabase* mDatabase;
	Model* mModel;
	Triangle* mTriangles;
	int mTriangleNumber;
};

#endif
