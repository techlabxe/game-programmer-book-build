#ifndef INCLUDED_STAGE_H
#define INCLUDED_STAGE_H

#include "Cuboid.h"

class Matrix44;
class GraphicsDatabase;
class Model;

class Stage{
public:
	Stage();
	~Stage();
	void draw( const Matrix44& perspectiveViewMatrix ) const;
	const Cuboid* cuboid() const;
private:
	GraphicsDatabase* mDatabase;
	Model* mModel;
	Cuboid mCuboid;
};

#endif
