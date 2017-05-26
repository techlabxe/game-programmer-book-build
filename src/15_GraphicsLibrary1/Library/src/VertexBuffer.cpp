#include "GameLib/GameLib.h"
#include "Library/VertexBuffer.h"
#include "Library/Vector3.h"
#include "Library/Vector2.h"

VertexBuffer::VertexBuffer( int size ) :
mPositions( 0 ),
mUVs( 0 ),
mSize( size ){
	mPositions = new Vector3[ mSize ];
	mUVs = new Vector2[ mSize ];
}

VertexBuffer::~VertexBuffer(){
	SAFE_DELETE_ARRAY( mPositions );
	SAFE_DELETE_ARRAY( mUVs );
}

const Vector3* VertexBuffer::position( int i ) const {
	ASSERT( i >= 0 && i < mSize );
	return &mPositions[ i ];
}

const Vector2* VertexBuffer::uv( int i ) const {
	ASSERT( i >= 0 && i < mSize );
	return &mUVs[ i ];
}

void VertexBuffer::setPosition( int i, const Vector3& p ){
	ASSERT( i >= 0 && i < mSize );
	mPositions[ i ] = p;
}

void VertexBuffer::setUV( int i, const Vector2& uv ){
	ASSERT( i >= 0 && i < mSize );
	mUVs[ i ] = uv;
}

int VertexBuffer::size() const {
	return mSize;
}
