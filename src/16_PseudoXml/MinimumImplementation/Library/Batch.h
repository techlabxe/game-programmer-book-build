#ifndef INCLUDED_BATCH_H
#define INCLUDED_BATCH_H

#include "GameLib/Framework.h"

#include <string>
using namespace std;

namespace PseudoXml{
	class Element;
}

class VertexBuffer;
class IndexBuffer;
class Matrix44;
class Texture;
class GraphicsDatabase;

class Batch{
public:
	Batch( const PseudoXml::Element*, const GraphicsDatabase& );
	Batch( 
		const VertexBuffer*,
		const IndexBuffer*,
		const Texture*,
		GameLib::Framework::BlendMode );
	~Batch();
	void draw( const Matrix44& transform ) const;
	const string* name() const;
private:
	const VertexBuffer* mVertexBuffer;
	const IndexBuffer* mIndexBuffer;
	const Texture* mTexture;
	GameLib::Framework::BlendMode mBlendMode;
	string mName;
};

#endif
