#ifndef INCLUDED_PSEUDO_XML_PARSER_H
#define INCLUDED_PSEUDO_XML_PARSER_H

#include <string>
#include <list>
using namespace std;

namespace PseudoXml{

class ElementData;

class Parser{
public:
	Parser( const char* data, int size );
	Element* build();
	~Parser();
private:
	int mMode; //モード変数
	bool mInEndTag; //終了タグにいるのか？
	list< ElementData* > mElementStack; //エレメントの材料が詰まっている
};

} //namespace PseudoXml

#endif