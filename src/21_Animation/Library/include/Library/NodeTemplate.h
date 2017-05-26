#ifndef INCLUDED_NODE_TEMPLATE_H
#define INCLUDED_NODE_TEMPLATE_H

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

class NodeTemplate{
public:
	NodeTemplate();
	~NodeTemplate();
	int child() const;
	int brother() const;
	const Batch* batch() const;
	const Vector3* translation() const;
	const Vector3* rotation() const;
	const Vector3* scale() const;
	const string* name() const;
private:
	friend class TreeTemplate; //TreeTemplateからはアクセスし放題

	int mChild;
	int mBrother;
	const Batch* mBatch;
	Vector3 mTranslation;
	Vector3 mRotation;
	Vector3 mScale; 
	string mName;
};

#endif
