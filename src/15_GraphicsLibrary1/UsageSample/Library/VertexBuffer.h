#ifndef INCLUDED_VERTEX_BUFFER_H
#define INCLUDED_VERTEX_BUFFER_H

class Vector3;
class Vector2;

class VertexBuffer{
public:
	VertexBuffer( int size );
	~VertexBuffer();
	const Vector3* position( int i ) const;
	const Vector2* uv( int i ) const;
	void setPosition( int i, const Vector3& );
	void setUV( int i, const Vector2& );
	int size() const;
private:
	Vector3* mPositions;
	Vector2* mUVs;
	int mSize;
};

#endif

