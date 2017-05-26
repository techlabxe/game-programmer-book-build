#ifndef INCLUDED_TEXTURE_H
#define INCLUDED_TEXTURE_H

#include "GameLib/Graphics/Texture.h"

#include <string>
using namespace std;

namespace GameLib{
	namespace PseudoXml{
		class Element;
	}
}

class Texture{
public:
	Texture( GameLib::PseudoXml::Element& );
	Texture( const char* filename );
	~Texture();
	void set() const; //ハードウェアにセット
	const string* name() const;
private:
	GameLib::Graphics::Texture mTexture;
	string mName;
};

#endif
