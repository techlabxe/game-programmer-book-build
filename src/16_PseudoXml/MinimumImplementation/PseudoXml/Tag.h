#ifndef INCLUDED_PSEUDO_XML_TAG_H
#define INCLUDED_PSEUDO_XML_TAG_H

#include <vector>
#include <string>
using namespace std;

namespace PseudoXml{

class Attribute;

class Tag{
public:
	enum Type{
		TYPE_BEGIN,
		TYPE_END,
	};
	Tag( const char* name ); //名前だけのタグを作る
	Tag( const char** readPointer, const char* end );
	~Tag();
	const string* name() const;
	//アトリビュート配列ゲット。重いからポインタ返し。
	const vector< Attribute* >* attributes() const;
	Type type() const;
private:
	vector< Attribute* > mAttributes;
	string mName;
	Type mType;
};

} //namespace PseudoXml

#endif
