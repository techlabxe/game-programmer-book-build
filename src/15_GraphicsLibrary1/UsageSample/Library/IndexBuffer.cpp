#include "GameLib/GameLib.h"
#include "Library/IndexBuffer.h"

IndexBuffer::IndexBuffer( int size ) :
mIndices( 0 ),
mSize( size ){
	mIndices = new unsigned[ mSize ];
}

IndexBuffer::~IndexBuffer(){
	SAFE_DELETE_ARRAY( mIndices );
}

unsigned IndexBuffer::index( int i ) const {
	ASSERT( i >= 0 && i < mSize );
	return mIndices[ i ];
}

void IndexBuffer::setIndex( int i, unsigned v ){
	ASSERT( i >= 0 && i < mSize );
	mIndices[ i ] = v;
}

int IndexBuffer::size() const {
	return mSize;
}
