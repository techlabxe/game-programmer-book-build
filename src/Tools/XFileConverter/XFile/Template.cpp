#include "GameLib/GameLib.h"
#include "XFile/Template.h"
#include "XFile/Token.h"
#include "XFile/Element.h"
#include "GameLib/Base/Tank.h"
#include "XFile/Parser.h"
#include <sstream>

namespace XFile{

using namespace GameLib;
using namespace std;

Template::Template( RefString name, const RefString& uuid, int childNumber ) :
mName( name ),
mChildren( childNumber ), 
mUuid( uuid ){
}

Template::~Template(){
}

void Template::setChild( int i, TemplateLine** tl ){
	mChildren[ i ] = *tl;
	*tl = 0;
}

const RefString* Template::getType() const {
	return &mName;
}

const RefString* Template::getChildName( int index ) const {
	if ( index >= mChildren.size() ){ //より大きい番号を要求されることがある。拡張部だ。0を返す。
		return 0;
	}else{
		return &mChildren[ index ]->mName;
	}
}

const RefString* Template::getChildType( int index ) const {
	if ( index >= mChildren.size() ){ //より大きい番号を要求されることがある。拡張部だ。0を返す。
		return 0;
	}else if ( mChildren[ index ]->mTemplate ){ //複合型
		return mChildren[ index ]->mTemplate->getType();
	}else{ //プリミティブ
		return &mChildren[ index ]->mPrimitiveType;
	}
}


int Template::createElement( 
Element** elementOut, 
const Array< Token >& t,
int p,
const Parser& parser,
const RefString& name,
map< RefString, Element* >* namedElements,
int* level ) const {

//if ( p % 1000 == 0 ){ cout << p << endl; }

	const int n = t.size();
	Tank< CompositeElement::Child > children;
	map< RefString, int > arrayIndices;
	//レギュラーデータ
	for ( int i = 0; i < mChildren.size(); ++i ){
		++( *level );
		CompositeElement::Child c; //子
		if ( ( p < n ) && ( t[ p ] == Token::LEFT_BRACE ) ){ //参照来ましたね...
			p = parseReference( &c.mElement, t, p, namedElements );
		}else{
			const TemplateLine& line = *mChildren[ i ];
			if ( line.mIndices.size() > 0 ){ //配列です
				p = createArray( &c.mElement, line, t, p, arrayIndices, parser, namedElements, level );
			}else if ( line.mTemplate ){ //複合型ですね
				RefString nullStr;
				p = line.mTemplate->createElement( &c.mElement, t, p, parser, nullStr, namedElements, level );
			}else{ //プリミティブです
				c.mPrimitive = *t[ p ].get();
				++p;
				int size = parser.toInt( c.mPrimitive ); //添え字になりうるか調べます
				if ( size > 0 ){
					arrayIndices.insert( make_pair( line.mName, size ) );
				}
			}
		}
		children.add( c );
		Parser::dieIf( p >= n );
		if ( t[ p ] == Token::SEMICOLON ){
			++p;
		}else{ //ありえない
#if 0 //メタセコイア対応。セミコロン抜けをスルー
			Parder::dieIf( true );
#endif
		}
		--( *level );
#if 1 //メタセコイア対応コード 余分なセミコロンを除去
		if ( ( *level == 0 ) && ( p < n ) && ( t[ p ] == Token::SEMICOLON ) ){ //ありえない
			++p;
		}
#endif
	}
	//追加データ
	while ( p < n ){
		CompositeElement::Child c;
		if ( t[ p ] == Token::LEFT_BRACE ){ //参照来ましたね...
			p = parseReference( &c.mElement, t, p, namedElements );
		}else if ( t[ p ] == Token::IDENTIFIER ){
			//型名を取り出します
			const RefString* typeName = t[ p ].get();
			++p;
			//変数名取り出し
			RefString childName;
			if ( ( p < n ) && ( t[ p ] == Token::IDENTIFIER ) ){
				childName = *t[ p ].get();
				++p;
			}
			//{
			Parser::dieIf( ( p >= n ) || ( t[ p ] != Token::LEFT_BRACE ) ); //ありえない
			++p;
			//テンプレートにデータを作ってもらいます。
			if ( parser.isPrimitiveType( *typeName ) ){ //プリミティブ
				c.mPrimitive = *t[ p ].get();
				++p;
			}else{ //複合型
				const Template* tmpl = parser.getTemplate( *typeName );
				int childLevel = 0;
				p = tmpl->createElement( &c.mElement, t, p, parser, childName, namedElements, &childLevel );
				namedElements->insert( make_pair( childName, c.mElement ) );
			}
			//}
			Parser::dieIf( ( p >= n ) || ( t[ p ] != Token::RIGHT_BRACE ) ); //ありえない
			++p;
		}else{
			break; //参照でもデータ実体でもない。つまり終わる。
		}
		children.add( c );
	}
	CompositeElement* e = NEW CompositeElement( this, children.size(), name );
	for ( int i = 0; !children.isEnd(); ++i ){
		CompositeElement::Child* c = children.get();
		e->setChild( i, *c );
		children.toNext();
	}
	*elementOut = e;

	return p;
}

int Template::createArray(
Element** elementOut, 
const TemplateLine& line,
const Array< Token >& t,
int p,
const map< RefString, int >& arrayIndices,
const Parser& parser,
map< RefString, Element* >* namedElements,
int* level ) const {
	//最初にやることは配列の添え字の次元と各次元のサイズを取得すること。ここは最適化の余地がある
	const int n = t.size();
	const Array< RefString >& indices = line.mIndices;
	int dimension = indices.size();
	Array< int > arraySizes( dimension );
	int k = 0;
	int arraySize = 1;
	for ( int i = 0; i < indices.size(); ++i ){
		const RefString& s = indices[ i ];
		int size = parser.toInt( s );
		if ( size > 0 ){
			arraySizes[ k ] = size;
		}else{
			map< RefString, int >::const_iterator l = arrayIndices.find( s );
			Parser::dieIf( l == arrayIndices.end() );
			arraySizes[ k ] = l->second;
		}
		arraySize *= arraySizes[ k ];
	}
	//プリミティブ配列か複合型かで分岐
	if ( line.mTemplate ){ //複合型
		CompositeArrayElement* a = NEW CompositeArrayElement( arraySizes );
		for ( int i = 0; i < arraySize; ++i ){
			Element* te = 0;
			RefString nullStr;
			p = line.mTemplate->createElement( &te, t, p, parser, nullStr, namedElements, level );
			if ( i < arraySize - 1 ){ //最終要素だけはカンマがない。なんてこった！
				Parser::dieIf( ( p >= n ) || ( t[ p ] != Token::COMMA ) );
				++p;
			}
			a->setElement( i, &te );
		}
		*elementOut = a;
	}else{ //プリミティブ
		PrimitiveArrayElement* a = NEW PrimitiveArrayElement( arraySizes, line.mPrimitiveType );
		for ( int i = 0; i < arraySize; ++i ){
			a->setElement( i, *t[ p ].get() );
			++p;
			if ( i < arraySize - 1 ){ //最終要素だけはカンマがない。なんてこった！
				Parser::dieIf( ( p >= n ) || ( t[ p ] != Token::COMMA ) );
				++p;
			}
		}
		*elementOut = a;
	}
	return p;
}

//参照処理 { name | UUID | name UUID }
int Template::parseReference(
Element** elementOut,
const Array< Token >& t,
int p,
map< RefString, Element* >* ) const {
	const int n = t.size();

	++p; //左かっこを飛ばす
	RefString refName;
	if ( ( p < n ) && ( t[ p ] == Token::IDENTIFIER ) ){
		refName = *t[ p ].get();
		++p;
	}
	if ( ( p < n ) && ( t[ p ] == Token::UUID ) ){ //UUIDは名前がない時だけ取ってくる。
		++p;
		if ( refName.size() == 0 ){
			refName = *t[ p ].get();
		}
	}
	Parser::dieIf( refName.size() == 0 );
	Parser::dieIf( ( p >= n ) || ( t[ p ] != Token::RIGHT_BRACE ) ); //ありえない
	++p;

	*elementOut = NEW ReferenceElement( refName );
/*
	//参照を差し込みます
	map< RefString, Element* >::iterator it = namedElements->find( refName );
	Parser::dieIf( it == namedElements->end() );
	*elementOut = it->second;
*/
	return p;
}


} //namespace XFile

