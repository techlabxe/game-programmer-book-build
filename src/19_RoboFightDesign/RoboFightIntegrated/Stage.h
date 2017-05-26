#ifndef INCLUDED_STAGE_H
#define INCLUDED_STAGE_H

class Matrix44;
class GraphicsDatabase;
class Model;

class Stage{
public:
	Stage();
	~Stage();
	void draw( const Matrix44& perspectiveViewMatrix ) const;
private:
	GraphicsDatabase* mDatabase;
	Model* mModel;
};

#endif
