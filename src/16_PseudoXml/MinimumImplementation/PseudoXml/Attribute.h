#ifndef INCLUDED_PSEUDO_XML_ATTRIBUTE_H
#define INCLUDED_PSEUDO_XML_ATTRIBUTE_H

#include <string>
using namespace std;

namespace PseudoXml{

class Attribute{
public:
	Attribute();
	Attribute( const char* name, const char* value );
	~Attribute();

	void set( const char* name, const char* value );
	void set( const char* name, int value ); //int値
	void set( const char* name, double value ); //double値
	void set( const char* name, const int* values, int number ); //int配列
	void set( const char* name, const double* values, int number ); //double配列
	const string* value() const;
	const string* name() const;
	//失敗すると0を返すが、もともと0が入っていたのとは区別がつかない。
	int getIntValue() const;
	//失敗すると0.0を返すが、もともと0.0が入っていたのとは区別がつかない。
	double getDoubleValue() const;
	//戻り値は実際に読めた要素数
	int getIntValues( int* out, int number ) const;
	//戻り値は実際に読めた要素数
	int getDoubleValues( double* out, int number ) const;
private:
	string mName;
	string mValue;
};

} //namespace PseudoXml

#endif
