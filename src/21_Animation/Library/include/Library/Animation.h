#ifndef INCLUDED_ANIMATION_H
#define INCLUDED_ANIMATION_H

#include <string>
using namespace std;

namespace GameLib{
	namespace PseudoXml{
		class Element;
	}
}

class AnimationNode;

class Animation{
public:
	Animation( GameLib::PseudoXml::Element& );
	~Animation();
	//ÉmÅ[ÉhÇÇ‡ÇÁÇ§
	const AnimationNode* node( const char* name ) const;
	const string* name() const;
private:
	AnimationNode** mNodes;
	int mNodeNumber;
	string mName;
};

#endif
