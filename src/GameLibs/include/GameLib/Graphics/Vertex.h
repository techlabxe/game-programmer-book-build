#ifndef INCLUDED_GAMELIB_GRAPHICS_VERTEX_H
#define INCLUDED_GAMELIB_GRAPHICS_VERTEX_H

#include "GameLib/Math/Vector4.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Vector2.h"

namespace GameLib{
using namespace Math;
namespace Graphics{

class Vertex{
public:
	Vector4 mPosition; //位置
	Vector3 mNormal; //法線
	unsigned mColor; //頂点色
	Vector2 mUv; //テクスチャ座標
};

} //namespace Graphics
} //namespace GameLib

#endif
