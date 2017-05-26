#ifndef INCLUDED_INDEX_BUFFER_H
#define INCLUDED_INDEX_BUFFER_H

#include <string>
#include "GameLib/Graphics/IndexBuffer.h"
using namespace std;

namespace GameLib{
	namespace PseudoXml{
		class Element;
	}
}

class IndexBuffer{
public:
	IndexBuffer( GameLib::PseudoXml::Element& );
	~IndexBuffer();
	int size() const;
	const string* name() const;
	void set() const;
private:
	GameLib::Graphics::IndexBuffer mIndexBuffer;
	int mSize;
	string mName;
};

#endif

