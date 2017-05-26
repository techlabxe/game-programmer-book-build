#ifndef INCLUDED_STAGE_H
#define INCLUDED_STAGE_H

#include "GameLib/Scene/Container.h"
#include "GameLib/Scene/Model.h"

namespace GameLib{
	namespace Math{
		class Vector3;
		class Matrix44;
	}
}
using namespace GameLib::Math;

class Stage{
public:
	Stage();
	~Stage();
	void draw() const;
private:
	GameLib::Scene::Container mSceneContainer;
	GameLib::Scene::Model mModel;
};

#endif
