#ifndef INCLUDED_GAMELIB_SCENE_PRIMITIVERENDERER_H
#define INCLUDED_GAMELIB_SCENE_PRIMITIVERENDERER_H

#include "GameLib/Graphics/Enum.h"

namespace GameLib{

namespace Graphics{
	class Texture;
}

namespace Math{
	class Matrix44;
	class Vector4;
	class Vector3;
	class Vector2;
}
using namespace Math;

namespace Scene{

///直接三角形、線分、点、2D長方形を描画するための便利クラス。速度的にはかなり無駄が多い。
class PrimitiveRenderer{
public:
	///頂点数と、その他命令の数を指定して構築
	static PrimitiveRenderer create( 
		int vertexCapacity, 
		int commandCapacity );

	void setTexture( Graphics::Texture );
	void enableDepthTest( bool );
	void enableDepthWrite( bool );
	void setBlendMode( Graphics::BlendMode );
	void setCullMode( Graphics::CullMode );
	void setTransform( const Matrix44& );

	///三角形を変換済み頂点で書く。つまりpは4要素ベクタ。
	void addTransformedTriangle(
		const Vector4& pos0,
		const Vector4& pos1,
		const Vector4& pos2,
		const Vector2& uv0,
		const Vector2& uv1,
		const Vector2& uv2,
		unsigned color0 = 0xffffffff,
		unsigned color1 = 0xffffffff,
		unsigned color2 = 0xffffffff);
	///三角形を変換済み頂点で書く。テクスチャなし。
	void addTransformedTriangle(
		const Vector4& pos0,
		const Vector4& pos1,
		const Vector4& pos2,
		unsigned color0 = 0xffffffff,
		unsigned color1 = 0xffffffff,
		unsigned color2 = 0xffffffff);
	///三角形を描く
	void addTriangle(
		const Vector3& pos0,
		const Vector3& pos1,
		const Vector3& pos2,
		const Vector2& uv0,
		const Vector2& uv1,
		const Vector2& uv2,
		unsigned color0 = 0xffffffff,
		unsigned color1 = 0xffffffff,
		unsigned color2 = 0xffffffff );
	///三角形を描く。テクスチャなし
	void addTriangle(
		const Vector3& pos0,
		const Vector3& pos1,
		const Vector3& pos2,
		unsigned color0 = 0xffffffff,
		unsigned color1 = 0xffffffff,
		unsigned color2 = 0xffffffff );
	///線分を描く。
	void addLine(
		const Vector3& pos0,
		const Vector3& pos1,
		const Vector2& uv0,
		const Vector2& uv1,
		unsigned color0 = 0xffffffff,
		unsigned color1 = 0xffffffff );
	///線分を描く。テクスチャなし。
	void addLine(
		const Vector3& pos0,
		const Vector3& pos1,
		unsigned color0 = 0xffffffff,
		unsigned color1 = 0xffffffff );
	///点を描く。
	void addPoint(
		const Vector3& pos,
		const Vector2& uv,
		unsigned color = 0xffffffff );
	///点を描く。テクスチャなし
	void addPoint(
		const Vector3& pos,
		unsigned color = 0xffffffff );
	///Z=0平面に長方形を描く。座標系はスクリーン座標。
	void addRectangle(
		const Vector2& pos0,
		const Vector2& pos1,
		const Vector2& uv0,
		const Vector2& uv1,
		unsigned color = 0xffffffff,
		float depth = 0.f );
	///Z=0平面に長方形を描く。座標系はスクリーン座標。テクスチャなし。
	void addRectangle(
		const Vector2& pos0,
		const Vector2& pos1,
		unsigned color = 0xffffffff,
		float depth = 0.f );
	///描画
	void draw();

	//以下ユーザは意識しない
	PrimitiveRenderer();
	PrimitiveRenderer( const PrimitiveRenderer& );
	~PrimitiveRenderer();
	operator void*() const;
	PrimitiveRenderer& operator=( const PrimitiveRenderer& );
	void release();
	bool operator==( const PrimitiveRenderer& ) const;
	bool operator!=( const PrimitiveRenderer& ) const;
private:
	class Impl;
	Impl* mImpl;
};

} //namespace Scene

} //namespace GameLib


#endif
