#ifndef INCLUDED_GAMELIB_SCENE_FONTIMPL_H
#define INCLUDED_GAMELIB_SCENE_FONTIMPL_H

#include "GameLib/Graphics/Texture.h"
#include "GameLib/Math/Vector2.h"
#include "GameLib/Base/Impl/ReferenceType.h"

namespace GameLib{
namespace Scene{

using namespace Graphics;
using namespace Math;

class Font::Impl : public ReferenceType{
public:
	Impl(){
	}
	virtual bool getChar( 
		Texture* textureOut,
		Vector2* uvTopLeftOut,
		Vector2* uvBottomRightOut,
		int code ) = 0;
	virtual int charHeight() const = 0;
	virtual ~Impl(){
	}
};

} //Scene
} //GameLib
#endif
