#include "GameLib/GameLib.h"
#include "PseudoXml/Tag.h"
#include "PseudoXml/Element.h"
#include "PseudoXml/Attribute.h"
using namespace std;

namespace PseudoXml{

Element::Element(){
}

Element::Element( const char* name ) :
mName( name ){
}

Element::Element( Tag* beginTag, const char** p, const char* e ){
	//名前を移植
	mName = *( beginTag->name() );
	//アトリビュートを移植(コピーでなくポインタをそのままもらって高速化)
	mAttributes = *beginTag->attributes(); //丸ごとコピー

	//タグ探す
	while ( *p < e ){
		if ( **p == '<' ){ //発見
			++( *p ); //<の次へ移動
			Tag tag( p, e ); //タグ解釈を丸投げ
			Tag::Type type = tag.type();
			if ( type == Tag::TYPE_BEGIN ){ //開始タグ
				mChildren.push_back( new Element( &tag, p, e ) ); //子に追加
			}else if ( type == Tag::TYPE_END ){ //終了タグ
				break; //終わる
			}
		}else{
			++( *p ); //<が出るまで無視しまくり
		}
	}
}

Element::~Element(){
	for ( unsigned i = 0; i < mAttributes.size(); ++i ){
		SAFE_DELETE( mAttributes[ i ] );
	}
	for ( unsigned i = 0; i < mChildren.size(); ++i ){
		SAFE_DELETE( mChildren[ i ] );
	}
}

int Element::attributeNumber() const {
	return static_cast< int >( mAttributes.size() );
}

const Attribute* Element::attribute( int i ) const {
	return mAttributes[ i ];
}

Attribute* Element::attribute( int i ){
	return mAttributes[ i ];
}

void Element::setAttributeNumber( int n ){
	//今ある分は捨てる
	for ( unsigned i = 0; i < mAttributes.size(); ++i ){
		SAFE_DELETE( mAttributes[ i ] );
	}
	mAttributes.resize( n );
	for ( int i = 0; i < n; ++i ){
		mAttributes[ i ] = new Attribute();
	}
}

int Element::childNumber() const {
	return static_cast< int >( mChildren.size() );
}

const Element* Element::child( int i ) const {
	return mChildren[ i ];
}

Element* Element::child( int i ){
	return mChildren[ i ];
}

void Element::setChildNumber( int n ){
	//今ある分は捨てる
	for ( unsigned i = 0; i < mChildren.size(); ++i ){
		SAFE_DELETE( mChildren[ i ] );
	}
	mChildren.resize( n );
	for ( int i = 0; i < n; ++i ){
		mChildren[ i ] = new Element();
	}
}

const string* Element::name() const {
	return &mName;
}

void Element::setName( const char* name ){
	mName = name;
}

void Element::convertToString( string* out, int indent ) const {
	//インデントの数だけタブを書く
	for ( int i = 0; i < indent; ++i ){
		*out += '\t';
	}
	//タグ開始とエレメント名
	*out += '<';
	*out += mName;
	//アトリビュート書き込み
	for ( unsigned i = 0; i < mAttributes.size(); ++i ){
		if ( mAttributes[ i ] ){
			*out += ' '; //スペース
			*out += *( mAttributes[ i ]->name() );
			*out += "=\"";
			*out += *( mAttributes[ i ]->value() );
			*out += '"';
		}
	}
	*out += ">\r\n"; //開始タグ終わり
	//子に流す
	for ( unsigned i = 0; i < mChildren.size(); ++i ){
		if ( mChildren[ i ] ){
			mChildren[ i ]->convertToString( out, indent + 1 );
		}
	}
	//インデントの数だけタブを書く
	for ( int i = 0; i < indent; ++i ){
		*out += '\t';
	}
	//終了タグ
	*out += "</";
	*out += mName;
	*out += ">\r\n";
}

} //namespace PseudoXml
