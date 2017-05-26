#ifndef INCLUDED_INDEX_BUFFER_H
#define INCLUDED_INDEX_BUFFER_H

#include <string>
using namespace std;

namespace GameLib{
	namespace PseudoXml{
		class Element;
	}
}

class IndexBuffer{
public:
	IndexBuffer( GameLib::PseudoXml::Element& );
	IndexBuffer( int size );
	~IndexBuffer();
	unsigned index( int i ) const;
	void setIndex( int i, unsigned indexValue );
	int size() const;
	const string* name() const;
private:
	unsigned* mIndices;
	int mSize;
	string mName;
};

#endif

