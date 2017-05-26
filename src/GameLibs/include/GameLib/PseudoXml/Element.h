#ifndef INCLUDED_GAMELIB_PSEUDOXML_ELEMENT_H
#define INCLUDED_GAMELIB_PSEUDOXML_ELEMENT_H

namespace GameLib{
class OStringStream;
namespace PseudoXml{
//using namespace std;

class Attribute;
class ConstAttribute;
class ConstElement;

class Element{
public:
	int attributeNumber() const;
	void setAttributeNumber( int );
	Attribute attribute( int i );
	ConstAttribute attribute( int i ) const;

	int childNumber() const;
	void setChildNumber( int );
	Element child( int i );
	ConstElement child( int i ) const;

	const char* name() const;
	void setName( const char* );

	void convertToString( OStringStream* out ) const;

	//以下使用者は意識しなくて良い。
	Element();
	Element( const Element& );
	~Element();
	operator void*() const;
	Element& operator=( const Element& );
	void release();
	bool operator==( const Element& ) const;
	bool operator!=( const Element& ) const;

	class Impl;
private:
	friend class Document;

	explicit Element( Impl* );
	Impl* mImpl;
};

class ConstElement{
public:
	int attributeNumber() const;
	ConstAttribute attribute( int i ) const;
	int childNumber() const;
	ConstElement child( int i ) const;
	const char* name() const;
	void convertToString( OStringStream* out ) const;

	//以下知らなくていい
	ConstElement();
	ConstElement( const Element& );
	~ConstElement();
	ConstElement& operator=( const Element& );
	void release();
	operator void*() const;
	bool operator==( const Element& ) const;
	bool operator!=( const Element& ) const;
private:
	Element mBase;
};

} //namespace PseudoXml
} //namespace GameLib
#endif
