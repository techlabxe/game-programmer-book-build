#ifndef INCLUDED_STAGE_H
#define INCLUDED_STAGE_H

#include "Sphere.h"

class Matrix44;
class GraphicsDatabase;
class Model;

class Stage{
public:
	Stage();
	~Stage();
	void draw( const Matrix44& perspectiveViewMatrix ) const;
	const Sphere* sphere() const;
private:
	GraphicsDatabase* mDatabase;
	Model* mModel;
	Sphere mSphere;
};

#endif
