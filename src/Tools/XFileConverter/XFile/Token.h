#ifndef INCLUDED_XFILE_TOKEN_H
#define INCLUDED_XFILE_TOKEN_H

#include "GameLib/Base/RefString.h"

namespace XFile{

using namespace GameLib;

//ÉgÅ[ÉNÉì
class Token{
public:
	enum Type{
		TEMPLATE,
		ARRAY,
		IDENTIFIER,
		UUID,
		SEMICOLON,
		COMMA,
		LEFT_BRACE,
		RIGHT_BRACE,
		BRACKETS,
		STRING,

		UNKNOWN,
	};
	Token() : mType( UNKNOWN ){}
	explicit Token( Type t ) : mType( t ){}
	explicit Token( Type t, const char* str, int size ) : mType( t ), mString( str, size ){
	}
	bool operator==( Type t ) const {
		return ( mType == t );
	}
	bool operator!=( Type t ) const {
		return ( mType != t );
	}
	const RefString* get() const { return &mString; }
private:
	Type mType;
	RefString mString;
};

} //namespace XFile

#endif