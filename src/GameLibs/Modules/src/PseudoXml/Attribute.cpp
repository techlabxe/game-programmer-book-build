#include "GameLib/PseudoXml/Attribute.h"
#include "PseudoXml/AttributeImpl.h"
#include "PseudoXml/DocumentImpl.h"

namespace GameLib{
namespace PseudoXml{

Attribute::Attribute() : mImpl( 0 ){
}

Attribute::Attribute( const Attribute& o ) : mImpl( o.mImpl ){
	if ( mImpl ){
		mImpl->document()->refer();
	}
}

Attribute::~Attribute(){
	release();
}

void Attribute::release(){
	if ( mImpl ){
		Document::Impl* d = mImpl->document();
		d->release();
		if ( d->referenceCount() == 0 ){
			SAFE_DELETE( d );
		}
	}
	mImpl = 0;
}

Attribute& Attribute::operator=( const Attribute& o ){
	release();
	mImpl = o.mImpl;
	if ( mImpl ){
		mImpl->document()->refer();
	}
	return *this;
}

bool Attribute::operator==( const Attribute& o ) const {
	return ( mImpl == o.mImpl );
}

bool Attribute::operator!=( const Attribute& o ) const {
	return ( mImpl != o.mImpl );
}

Attribute::operator void*() const {
	return ( mImpl == 0 ) ? 0 : reinterpret_cast< void* >( ~0 );
}


Attribute::Attribute( Impl* impl ) : mImpl( impl ){
	if ( mImpl ){
		mImpl->document()->refer();
	}
}

const char* Attribute::name() const { 
	return mImpl->name();
}

const char* Attribute::value() const { 
	return mImpl->value();
}

int Attribute::getIntValue() const {
	return mImpl->getIntValue();
}

float Attribute::getFloatValue() const {
	return mImpl->getFloatValue();
}

double Attribute::getDoubleValue() const {
	return mImpl->getDoubleValue();
}

int Attribute::getIntValues( int* out, int n ) const {
	return mImpl->getIntValues( out, n );
}

int Attribute::getFloatValues( float* out, int n ) const {
	return mImpl->getFloatValues( out, n );
}

int Attribute::getDoubleValues( double* out, int n ) const {
	return mImpl->getDoubleValues( out, n );
}

void Attribute::set( 
const char* name,
int nameSize,
const char* value,
int valueSize ){
	mImpl->setName( name, nameSize );
	mImpl->setValue( value, valueSize );
}

void Attribute::set( const char* name, const char* value ){
	mImpl->setName( name );
	mImpl->setValue( value );
}

void Attribute::set( const char* name, int value ){
	mImpl->setName( name );
	mImpl->setValue( value );
}

void Attribute::set( const char* name, float value ){
	mImpl->setName( name );
	mImpl->setValue( value );
}

void Attribute::set( const char* name, double value ){
	mImpl->setName( name );
	mImpl->setValue( value );
}

void Attribute::set( const char* name, const int* values, int n ){
	mImpl->setName( name );
	mImpl->setValue( values, n );
}

void Attribute::set( const char* name, const float* values, int n ){
	mImpl->setName( name );
	mImpl->setValue( values, n );
}

void Attribute::set( const char* name, const double* values, int n ){
	mImpl->setName( name );
	mImpl->setValue( values, n );
}

#define TYPE Attribute
#define CONST_TYPE ConstAttribute
#include "GameLib/Base/Impl/ConstantTypeTemplate.h"

//ConstAttribute
const char* ConstAttribute::value() const {
	return mBase.value();
}

const char* ConstAttribute::name() const {
	return mBase.name();
}

int ConstAttribute::getIntValue() const {
	return mBase.getIntValue();
}

float ConstAttribute::getFloatValue() const {
	return mBase.getFloatValue();
}

double ConstAttribute::getDoubleValue() const {
	return mBase.getDoubleValue();
}

int ConstAttribute::getIntValues( int* out, int number ) const {
	return mBase.getIntValues( out, number );
}

int ConstAttribute::getFloatValues( float* out, int number ) const {
	return mBase.getFloatValues( out, number );
}

int ConstAttribute::getDoubleValues( double* out, int number ) const {
	return mBase.getDoubleValues( out, number );
}

} //namespace PseudoXml
} //namespace GameLib
