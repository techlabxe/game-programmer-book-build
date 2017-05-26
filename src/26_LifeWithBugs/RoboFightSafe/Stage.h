#ifndef INCLUDED_STAGE_H
#define INCLUDED_STAGE_H

namespace GameLib{
	namespace Math{
		class Vector3;
		class Matrix44;
	}
}
using namespace GameLib;
using namespace GameLib::Math;

class GraphicsDatabase;
class Model;

class Stage{
public:
	Stage();
	~Stage();
	void draw() const; 
private:
	AutoPtr< GraphicsDatabase > mDatabase;
	AutoPtr< Model > mModel;
};

#endif
