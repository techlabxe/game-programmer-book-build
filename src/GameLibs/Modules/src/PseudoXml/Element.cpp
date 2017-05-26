#include "GameLib/GameLib.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"

#include "PseudoXml/ElementImpl.h"
#include "PseudoXml/DocumentImpl.h"

namespace GameLib{
namespace PseudoXml{


Element::Element() : mImpl( 0 ){
}

Element::Element( const Element& o ) : mImpl( o.mImpl ){
	if ( mImpl ){
		mImpl->document()->refer();
	}
}

Element::~Element(){
	release();
}

void Element::release(){
	if ( mImpl ){
		Document::Impl* d = mImpl->document();
		d->release();
		if ( d->referenceCount() == 0 ){
			SAFE_DELETE( d );
		}
	}
	mImpl = 0;
}

Element& Element::operator=( const Element& o ){
	release();
	mImpl = o.mImpl;
	if ( mImpl ){
		mImpl->document()->refer();
	}
	return *this;
}

bool Element::operator==( const Element& o ) const {
	return ( mImpl == o.mImpl );
}

bool Element::operator!=( const Element& o ) const {
	return ( mImpl != o.mImpl );
}

Element::operator void*() const {
	return ( mImpl == 0 ) ? 0 : reinterpret_cast< void* >( ~0 );
}

Element::Element( Impl* impl ) : mImpl( impl ){
	if ( mImpl ){
		mImpl->document()->refer();
	}
}

int Element::attributeNumber() const {
	return mImpl->attributeNumber();
}

void Element::setAttributeNumber( int n ){
	mImpl->setAttributeNumber( n );
}

Attribute Element::attribute( int i ){
	return Attribute( mImpl->attribute( i ) );
}

ConstAttribute Element::attribute( int i ) const {
	return Attribute( mImpl->attribute( i ) );
}

int Element::childNumber() const { 
	return mImpl->childNumber();
}

void Element::setChildNumber( int n ){
	mImpl->setChildNumber( n );
}

Element Element::child( int i ){
	return Element( mImpl->child( i ) );
}

ConstElement Element::child( int i ) const {
	return Element( mImpl->child( i ) );
}

const char* Element::name() const { 
	return mImpl->name();
}

void Element::setName( const char* name ){
	mImpl->setName( name );
}

void Element::convertToString( OStringStream* out ) const {
	mImpl->convertToString( out, 0 );
}

//ConstElement
#define TYPE Element
#define CONST_TYPE ConstElement
#include "GameLib/Base/Impl/ConstantTypeTemplate.h"

int ConstElement::attributeNumber() const {
	return mBase.attributeNumber();
}

ConstAttribute ConstElement::attribute( int i ) const {
	return mBase.attribute( i );
}

int ConstElement::childNumber() const {
	return mBase.childNumber();
}

ConstElement ConstElement::child( int i ) const {
	return mBase.child( i );
}

const char* ConstElement::name() const {
	return mBase.name();
}

void ConstElement::convertToString( OStringStream* out ) const {
	mBase.convertToString( out );
}

} //namespace PseudoXml
} //namespace GameLib
