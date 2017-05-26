#include "PseudoXml/Attribute.h"
#include <cstdlib>
#include <sstream>
using namespace std;

namespace PseudoXml{

Attribute::Attribute(){
}

Attribute::Attribute( const char* name, const char* value ) :
mName( name ),
mValue( value ){
}

void Attribute::set( const char* name, const char* value ){
	mName = name;
	mValue = value;
}

void Attribute::set( const char* name, int value ){
	mName = name;
	ostringstream oss;
	oss << value;
	mValue = oss.str();
}

void Attribute::set( const char* name, double value ){
	mName = name;
	ostringstream oss;
	oss << value;
	mValue = oss.str();
}

void Attribute::set( const char* name, const int* values, int n ){
	mName = name;
	ostringstream oss;
	if ( n > 0 ){
		for ( int i = 0; i < ( n - 1 ); ++i ){
			oss << values[ i ] << ", ";
		}
		oss << values[ n - 1 ];
	}
	mValue = oss.str();
}

void Attribute::set( const char* name, const double* values, int n ){
	mName = name;
	ostringstream oss;
	if ( n > 0 ){
		for ( int i = 0; i < ( n - 1 ); ++i ){
			oss << values[ i ] << ", ";
		}
		oss << values[ n - 1 ];
	}
	mValue = oss.str();
}

Attribute::~Attribute(){
}

const string* Attribute::name() const {
	return &mName;
}

const string* Attribute::value() const {
	return &mValue;
}

int Attribute::getIntValue() const {
	istringstream iss( mValue );
	int r = 0;
	iss >> r;
	return r;
}

double Attribute::getDoubleValue() const {
	istringstream iss( mValue );
	double r = 0;
	iss >> r;
	return r;
}

int Attribute::getIntValues( int* a, int n ) const {
	istringstream iss( mValue );
	int c = 0;
	int l = static_cast< int >( mValue.size() );
	for ( int i = 0; i < n; ++i ){
		iss >> a[ i ];
		if ( iss.fail() ){
			break;
		}
		++c; //読み出し成功
		iss.ignore( l, ',' ); //カンマまで読み飛ばす
	}
	return c;
}

int Attribute::getDoubleValues( double* a, int n ) const {
	istringstream iss( mValue );
	int c = 0;
	int l = static_cast< int >( mValue.size() );
	for ( int i = 0; i < n; ++i ){
		iss >> a[ i ];
		if ( iss.fail() ){
			break;
		}
		++c; //読み出し成功
		iss.ignore( l, ',' ); //カンマまで読み飛ばす
	}
	return c;
}




} //namespace PseudoXml