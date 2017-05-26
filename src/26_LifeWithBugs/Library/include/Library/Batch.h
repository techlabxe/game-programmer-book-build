#ifndef INCLUDED_BATCH_H
#define INCLUDED_BATCH_H

#include "GameLib/Graphics/Enum.h"
#include "GameLib/Math/Vector3.h"
#include <string>
using namespace std;

namespace GameLib{
	namespace PseudoXml{
		class Element;
	}
	namespace Math{
		class Matrix44;
	}
}
using namespace GameLib::Math;

class VertexBuffer;
class IndexBuffer;
class Texture;
class GraphicsDatabase;

class Batch{
public:
	Batch( GameLib::PseudoXml::Element&, const GraphicsDatabase& );
	~Batch();
	void draw() const;
	const string* name() const;
private:
	ConstSharedPtr< VertexBuffer > mVertexBuffer;
	ConstSharedPtr< IndexBuffer > mIndexBuffer;
	ConstSharedPtr< Texture > mTexture;

	GameLib::Graphics::BlendMode mBlendMode;
	string mName;
	Vector3 mDiffuseColor;
	Vector3 mSpecularColor;
	float mTransparency;
	float mSpecularSharpness;
};

#endif
