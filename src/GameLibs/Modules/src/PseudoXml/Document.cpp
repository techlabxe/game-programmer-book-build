#include "GameLib/GameLib.h"
#include "GameLib/PseudoXml/Document.h"
#include "GameLib/PseudoXml/Element.h"

#include "PseudoXml/AttributeImpl.h"
#include "PseudoXml/Tag.h"
#include "PseudoXml/ElementImpl.h"
#include "PseudoXml/DocumentImpl.h"
#include <string>

namespace GameLib{
namespace PseudoXml{
using namespace std;

Document Document::create(){
	Document r;
	r.mImpl = NEW Impl();
	return r;
}

Document Document::create( const char* filename ){
	Document r;
	r.mImpl = NEW Impl();
	r.mImpl->load( filename );
	return r;
}

Document Document::create( const char* data, int size ){
	Document r;
	r.mImpl = NEW Impl();
	r.mImpl->build( data, size );
	return r;
}

void Document::write( const char* filename, bool isBinary ) const {
	if ( isBinary ){
		Array< char > out;
		mImpl->convertToBinary( &out );
		FileIO::Manager().write( 
			filename, 
			&out[ 0 ],
			out.size() );
	}else{
		string out;
		mImpl->convertToString( &out );
		FileIO::Manager().write( 
			filename, 
			out.c_str(), 
			static_cast< int >( out.size() ) );
	}
}

void Document::convertToString( string* out ) const {
	STRONG_ASSERT( mImpl && "This is Empty Object." );
	mImpl->convertToString( out );
}

void Document::convertToBinary( Array< char >* out ) const {
	STRONG_ASSERT( mImpl && "This is Empty Object." );
	mImpl->convertToBinary( out );
}

Element Document::root(){
	return Element( mImpl->root() );
}

ConstElement Document::root() const {
	return Element( mImpl->root() );
}

bool Document::isReady(){
	return ( mImpl && mImpl->isReady() );
}

bool Document::isError() const {
	return ( mImpl && mImpl->isError() );
}

int Document::childNumber() const {
	return mImpl->root()->childNumber();
}

Element Document::child( int i ){
	return Element( mImpl->root()->child( i ) );
}

ConstElement Document::child( int i ) const {
	return Element( mImpl->root()->child( i ) );
}

#define TYPE Document
#include "GameLib/Base/Impl/ReferenceTypeTemplate.h"
#define TYPE Document
#define CONST_TYPE ConstDocument
#include "GameLib/Base/Impl/ConstantTypeTemplate.h"

//ConstDocument
ConstElement ConstDocument::root() const {
	return mBase.root();
}

bool ConstDocument::isError() const {
	return mBase.isError();
}

void ConstDocument::convertToString( string* out ) const {
	mBase.convertToString( out );
}

void ConstDocument::convertToBinary( Array< char >* out ) const {
	mBase.convertToBinary( out );
}

void ConstDocument::write( const char* filename, bool isBinary ) const {
	mBase.write( filename, isBinary );
}

int ConstDocument::childNumber() const {
	return mBase.childNumber();
}

ConstElement ConstDocument::child( int i ) const {
	return mBase.child( i );
}

} //namespace PseudoXml
} //namespace GameLib
