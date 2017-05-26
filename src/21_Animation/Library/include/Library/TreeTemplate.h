#ifndef INCLUDED_TREE_TEMPLATE_H
#define INCLUDED_TREE_TEMPLATE_H

namespace GameLib{
	namespace PseudoXml{
		class Element;
	}
}

#include <string>
using namespace std;

#include "Library/Vector3.h"
class Matrix44;
class Batch;
class GraphicsDatabase;
class NodeTemplate;

class TreeTemplate{
public:
	TreeTemplate( GameLib::PseudoXml::Element&, const GraphicsDatabase& );
	~TreeTemplate();
	int nodeNumber() const;
	const NodeTemplate* node( int i ) const;
	const string* name() const;
private:
	int countNode( GameLib::PseudoXml::Element&, int n );
	int build( GameLib::PseudoXml::Element&, const GraphicsDatabase& db, int nodePos );

	NodeTemplate* mNodes; //0番が根と決まっているので根を指すポインタは不要
	int mNodeNumber;
	string mName;
};

#endif
