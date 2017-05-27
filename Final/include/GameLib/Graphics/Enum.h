#ifndef INCLUDED_GAMELIB_GRAPHICS_ENUM_H
#define INCLUDED_GAMELIB_GRAPHICS_ENUM_H

namespace GameLib{
namespace Graphics{

//ÉOÉçÅ[ÉoÉãóÒãìåQ
enum BlendMode{
	BLEND_OPAQUE,
	BLEND_LINEAR,
	BLEND_ADDITIVE,

	BLEND_MAX,
};

enum CullMode{
	CULL_NONE,
	CULL_FRONT,
	CULL_BACK,

	CULL_MAX,
};

enum PrimitiveType{
	PRIMITIVE_TRIANGLE,
	PRIMITIVE_LINE,
	PRIMITIVE_POINT,
};

enum TextureFilter{
	TEXTURE_FILTER_POINT,
	TEXTURE_FILTER_LINEAR,
};

enum LightingMode{
	LIGHTING_NONE,
	LIGHTING_PER_VERTEX,
	LIGHTING_PER_PIXEL,
};

} //namespace Graphics
} //namespace GameLib

#endif
