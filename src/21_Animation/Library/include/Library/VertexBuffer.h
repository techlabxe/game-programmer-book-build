#ifndef INCLUDED_VERTEX_BUFFER_H
#define INCLUDED_VERTEX_BUFFER_H

#include <string>
using namespace std;

namespace GameLib{
	namespace PseudoXml{
		class Element;
	}
}

class Vector3;
class Vector2;

class VertexBuffer{
public:
	VertexBuffer( GameLib::PseudoXml::Element& );
	VertexBuffer( int size );
	~VertexBuffer();
	const Vector3* position( int i ) const;
	const Vector2* uv( int i ) const;
	void setPosition( int i, const Vector3& );
	void setUV( int i, const Vector2& );
	int size() const;
	const string* name() const;
private:
	Vector3* mPositions;
	Vector2* mUVs;
	int mSize;
	string mName;
};

#endif

