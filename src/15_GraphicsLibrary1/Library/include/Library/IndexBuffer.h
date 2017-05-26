#ifndef INCLUDED_INDEX_BUFFER_H
#define INCLUDED_INDEX_BUFFER_H

class IndexBuffer{
public:
	IndexBuffer( int size );
	~IndexBuffer();
	unsigned index( int i ) const;
	void setIndex( int i, unsigned indexValue );
	int size() const;
private:
	unsigned* mIndices;
	int mSize;
};

#endif

