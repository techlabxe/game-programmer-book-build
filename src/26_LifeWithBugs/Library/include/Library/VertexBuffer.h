#ifndef INCLUDED_VERTEX_BUFFER_H
#define INCLUDED_VERTEX_BUFFER_H

#include "GameLib/Graphics/VertexBuffer.h"
#include <string>
using namespace std;

namespace GameLib{
	namespace PseudoXml{
		class Element;
	}
	namespace Math{
		class Vector3;
		class Vector2;
	}
}
using namespace GameLib::Math;

class VertexBuffer{
public:
	VertexBuffer( GameLib::PseudoXml::Element& );
	~VertexBuffer();
	int size() const;
	const string* name() const;
	void set() const;
private:
	GameLib::Graphics::VertexBuffer mVertexBuffer;
	int mSize;
	string mName;
};

#endif

