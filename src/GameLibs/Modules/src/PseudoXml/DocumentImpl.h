#ifndef INCLUDED_GAMELIB_PSEUDOXML_DOCUMENTIMPL_H
#define INCLUDED_GAMELIB_PSEUDOXML_DOCUMENTIMPL_H

#include "GameLib/FileIO/InFile.h"
#include "GameLib/FileIO/Manager.h"
#include "GameLib/Base/Impl/ReferenceType.h"
#include "GameLib/Base/Pool.h"
#include "GameLib/Base/OStringStream.h"
#include "GameLib/Base/IBinaryStream.h"
#include "GameLib/PseudoXml/Document.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"

namespace GameLib{
namespace PseudoXml{
using namespace std;

class Document::Impl : public ReferenceType{
public:
	void load( const char* filename ){
		mFile = FileIO::InFile::create( filename );
	}
	bool isError() const {
		return ( mFile && mFile.isError() );
	}
	const char* allocateString( const char* name ){
		return mStringPool.allocate( name, static_cast< int >( strlen( name ) ) + 1 );
	}
	const char* allocateString( const RefString& name ){
		char* p = mStringPool.allocate( name.get(), name.size() + 1 );
		p[ name.size() ] = '\0';
		return p;
	}
	const char* allocateString( const string& name ){
		return mStringPool.allocate( name.c_str(), static_cast< int >( name.size() ) + 1 );
	}
	const char* allocateString( const char* name, int size ){
		char* p = mStringPool.allocate( name, size + 1 );
		p[ size ] = '\0';
		return p;
	}
	//à»â∫cppÇ…ÇƒíËã`ÅBèzä¬éQè∆ÇîÇØÇÈÇΩÇﬂ
	Impl();
	~Impl();
	Element::Impl* allocateElement( int n );
	Attribute::Impl* allocateAttribute( int n );
	Element::Impl* root();
	bool isReady();
	void build( const char* data, int size );
	void convertToString( string* out ) const;
	void convertToBinary( Array< char >* out ) const;
private:
	Pool< char > mStringPool;
	Pool< Element::Impl > mElementPool;
	Pool< Attribute::Impl > mAttributePool;
	Element::Impl* mRoot;
	FileIO::InFile mFile;
};

} //namespace PseudoXml
} //namespace GameLib

#endif
