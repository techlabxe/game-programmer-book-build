#ifndef INCLUDED_XFILE_TEMPLATE_H
#define INCLUDED_XFILE_TEMPLATE_H

#include "GameLib/Base/RefString.h"
#include "GameLib/Base/Array.h"
#include "GameLib/Base/AutoPtr.h"
#include "XFile/Parser.h"
#include <map>

namespace XFile{

class Token;
class Element;
class TemplateLine;
using namespace GameLib;
using namespace std;

class CompositeElement;
class Template{
public:
	Template( RefString name, const RefString& uuid, int childNumber );
	void setChild( int i, TemplateLine** tl );
	~Template();

	int createElement( 
		Element** elementOut, 
		const Array< Token >& t,
		int p,
		const Parser&,
		const RefString& name,
		map< RefString, Element* >* namedElements,
		int* level ) const;
	const RefString* getChildName( int index ) const;
	const RefString* getChildType( int index ) const;
	const RefString* getType() const;
private:
	void operator=( const Template& ); //‹ÖŽ~
	Template( const Template& ); //‹ÖŽ~

	int createArray(
		Element** elementOut, 
		const TemplateLine& line,
		const Array< Token >& t,
		int p,
		const map< RefString, int >& arrayIndices,
		const Parser&,
		map< RefString, Element* >* namedElements,
		int* level ) const;
	int parseReference(
		Element** elementOut,
		const Array< Token >& t,
		int p,
		map< RefString, Element* >* namedElements ) const;

	RefString mName;
	Array< AutoPtr< TemplateLine > > mChildren;
	const RefString mUuid;
};

} //namespace XFile

#endif
