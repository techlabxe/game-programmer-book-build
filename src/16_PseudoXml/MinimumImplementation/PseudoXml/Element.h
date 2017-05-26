#ifndef INCLUDED_PSEUDO_XML_ELEMENT_H
#define INCLUDED_PSEUDO_XML_ELEMENT_H

#include <string>
#include <vector>
using namespace std;

namespace PseudoXml{

class Tag;
class Attribute;

class Element{
public:
	Element();
	Element( const char* name ); //名前からコンストラクト
	Element( Tag*, const char** readPointer, const char* end );
	~Element();
	//アトリビュートゲット系
	int attributeNumber() const;
	const Attribute* attribute( int i ) const;
	Attribute* attribute( int i );
	//アトリビュートセット系
	void setAttributeNumber( int );
	//子供ゲット系
	int childNumber() const;
	const Element* child( int i ) const;
	Element* child( int i );
	//子供セット系
	void setChildNumber( int );
	//名前ゲット
	const string* name() const;
	//名前変更
	void setName( const char* );
	//文字列化
	void convertToString( string* out, int indent ) const;
private:
	vector< Attribute* > mAttributes;
	vector< Element* > mChildren;
	string mName;
};

} //namespace PseudoXml

#endif
