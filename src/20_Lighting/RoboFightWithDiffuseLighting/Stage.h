#ifndef INCLUDED_STAGE_H
#define INCLUDED_STAGE_H

class Matrix44;
class GraphicsDatabase;
class Model;
class Vector3;

class Stage{
public:
	Stage();
	~Stage();
	void draw( 
		const Matrix44& perspectiveViewMatrix,
		const Vector3& lightVector,
		const Vector3& lightColor,
		const Vector3& ambient ) const;
private:
	GraphicsDatabase* mDatabase;
	Model* mModel;
};

#endif
