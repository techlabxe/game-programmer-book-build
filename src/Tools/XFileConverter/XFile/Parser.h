#ifndef INCLUDED_XFILE_PARSER_H
#define INCLUDED_XFILE_PARSER_H

#include <map>
#include "GameLib/Base/RefString.h"
#include "GameLib/Base/Tank.h"
#include "GameLib/Base/Array.h"
#include "GameLib/Base/HashMap.h"
#include "GameLib/Base/HashSet.h"
#include "GameLib/Base/AutoPtr.h"
#include "XFile/Token.h"

namespace XFile{

using namespace GameLib;

class Template;
class Element;

//1行のエントリを表すクラス
class TemplateLine{
public:
	const Template* mTemplate;
	RefString mPrimitiveType;
	RefString mName;
	Array< RefString > mIndices;
private:
	void operator=( const TemplateLine& ); //代入禁止
};

class SyntaxErrorException{
public:
	SyntaxErrorException(){ ASSERT( false ); } //デバグ版即死装置
}; //文法エラー例外

class Parser{
public:
	Parser( const char* data, int size );
	~Parser();
	bool isValid() const;
	void toString( string* ) const;
	bool isPrimitiveType( const RefString& typeName ) const;
	const Template* getTemplate( const RefString& typeName ) const;
	static void dieIf( bool );
	static int toInt( const RefString& );
private:

	bool checkHeader( const char* data, const char* end );
	int preprocess( char* dst, const char* src, int size );
	void tokenize( Tank< Token >*, const char* data, int size );
	void analyze( const Array< Token >& );
	void analyzeFile( const Array< Token >& );
	int analyzeTemplate( const Array< Token >&, int pos, map< RefString, Template* >* );
	int analyzeTemplateEntry( TemplateLine*, const Array< Token >&, int pos, map< RefString, Template* >* );
	int analyzeSingleTemplate( TemplateLine*, const Array< Token >&, int pos, map< RefString, Template* >* );
	int analyzeArrayTemplate( TemplateLine*, const Array< Token >&, int pos, map< RefString, Template* >* );

	bool mIsValid;

	HashMap< RefString, Template* > mTemplates;
	HashSet< RefString > mPrimitiveTypes;
	Element* mRootElement;
	Array< char > mPreprocessed;
};

} //namespace XFile

#endif