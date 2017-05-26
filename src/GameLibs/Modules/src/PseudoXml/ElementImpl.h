#ifndef INCLUDED_GAMELIB_PSEUDOXML_ELEMENTIMPL_H
#define INCLUDED_GAMELIB_PSEUDOXML_ELEMENTIMPL_H

#include "GameLib/Base/Vector.h"
#include "GameLib/Base/OStringStream.h"
#include "GameLib/Base/IBinaryStream.h"
#include "PseudoXml/DocumentImpl.h"
#include "PseudoXml/Tag.h"
#include <map>
using namespace std;

namespace GameLib{
namespace PseudoXml{

class Element::Impl{
public:
	Impl() : 
	mChildren( 0 ),
	mChildNumber( 0 ),
	mAttributes( 0 ),
	mAttributeNumber( 0 ),
	mName( 0 ),
	mDocument( 0 ){
	}
	~Impl(){
		//全部Documentが実体を持っているのでここでは何もしない
		mChildren = 0;
		mAttributes = 0;
		mName = 0;
		mDocument = 0;
	}
	void build( Tag* tag, const char** p, const char* e ){
		//これは実質コンストラクタなので、二度目の呼び出しは不正
		ASSERT( mAttributeNumber == 0 && mChildNumber == 0 );
		//名前を移植

		mName = mDocument->allocateString( *tag->name() );

		//アトリビュートを移植
		mAttributeNumber = tag->attributeNumber();
		mAttributes = mDocument->allocateAttribute( mAttributeNumber );
		tag->giveAttributes( mAttributes );

		//空要素タグなら子はいないのでここで抜ける。
		if ( tag->type() == Tag::TYPE_BEGIN_END ){
			return;
		}

		//一時子エレメントリスト
		Tank< Element::Impl > children;
		//タグ探す
		while ( *p < e ){
			if ( **p == '<' ){ //発見
				++( *p ); //<の次へ移動
				Tag tmpTag( p, e, mDocument ); //タグ解釈を丸投げ
				Tag::Type type = tmpTag.type();
				if ( type == Tag::TYPE_END ){ //終了タグなら終わり。抜ける。
					break; //終わる
				}else if ( type == Tag::TYPE_COMMENT ){
					; //何もしない。
				}else{ //開始、空要素どちらでもこの処理は必要
					Element::Impl* tmpE = children.add(); //子に追加
					tmpE->setDocument( mDocument );
					tmpE->build( &tmpTag, p, e );
					tmpE = 0;
				}
			}else{
				++( *p ); //<が出るまで無視しまくり
			}
		}
		//子エレメントを本確保
		mChildNumber = children.size();
		mChildren = mDocument->allocateElement( mChildNumber );
		children.copyTo( mChildren );
	}
	void build( IBinaryStream* s, const char* stringBuffer ){
		mName = stringBuffer + s->readInt();
		mAttributeNumber = s->readInt();
		mAttributes = mDocument->allocateAttribute( mAttributeNumber );
		for ( int i = 0; i < mAttributeNumber ; ++i){
			const char* name = stringBuffer + s->readInt();
			const char* value = stringBuffer + s->readInt();
			mAttributes[ i ].setDocument( mDocument );
			mAttributes[ i ].setNameReference( name );
			if ( *value != '\0' ){
				mAttributes[ i ].setValueReference( value );
			}
		}
		mChildNumber = s->readInt();
		mChildren = mDocument->allocateElement( mChildNumber );
		for ( int i = 0; i < mChildNumber; ++i ){
			mChildren[ i ].setDocument( mDocument );
			mChildren[ i ].build( s, stringBuffer );
		}
	}
	//単純に一個づつ呼んで行く。
	void convertToString( OStringStream* out, int indent ) const {
		//インデントの数だけタブを書く
		for ( int i = 0; i < indent; ++i ){
			*out << '\t';
		}
		//タグ開始とエレメント名
		*out << '<' << mName;
		//アトリビュート書き込み
		//5個を境に出し方を変える
		int an = mAttributeNumber;
		if ( an >= 5 ){
			*out << "\r\n";
			//多いから全部改行
			for ( int i = 0; i < an; ++i ){
				//インデントの数だけタブを書く
				for ( int j = 0; j < indent; ++j ){
					*out << '\t';
				}
				*out << mAttributes[ i ].name();
				const char* value = mAttributes[ i ].value();
				if ( value ){
					*out << "=\"" << mAttributes[ i ].value() << '"';
				}
				if ( i != an - 1 ){
					*out << "\r\n"; //最後は改行しない
				}
			}
		}else{
			for ( int i = 0; i < an; ++i ){
				*out << ' '; //スペース
				*out << mAttributes[ i ].name();
				const char* value = mAttributes[ i ].value();
				if ( value ){
					*out << "=\"" << mAttributes[ i ].value() << '"';
				}
			}
		}
		if ( mChildNumber > 0 ){ //子がいる場合
			*out << ">\r\n"; //開始タグ終わり
			//子に流す
			for ( int i = 0; i < mChildNumber; ++i ){
				//名前がセットされていれば。
				if ( mChildren[ i ].name() ){
					mChildren[ i ].convertToString( out, indent + 1 );
				}
			}
			//インデントの数だけタブを書く
			for ( int i = 0; i < indent; ++i ){
				*out << '\t';
			}
			//終了タグ
			*out << "</" << mName << ">\r\n";
		}else{ //子がいないなら空要素タグでよかろう
			*out << "/>\r\n";
		}
	}
	void convertToBinary( 
	OStringStream* out, 
	OStringStream* stringBuffer,
	map< RefString, int >* stringMap ) const {
		typedef map< RefString, int > Map;
		RefString name( mName );
		Map::iterator it = stringMap->find( name );
		int pos = 0;
		if ( it == stringMap->end() ){
			pos = stringBuffer->size();
			*stringBuffer << name;
			stringBuffer->write( "\0", 1 );
			stringMap->insert( make_pair( name, pos ) );
		}else{
			pos = it->second;
		}
		out->write( pos ); //nameOffset
		out->write( mAttributeNumber ); //attributeNumber
		for ( int i = 0; i < mAttributeNumber; ++i ){
			const Attribute::Impl& a = mAttributes[ i ];
			RefString aName( a.name() );
			RefString aValue;
			if ( a.value() ){
				aValue = a.value();
			}
			//name
			it = stringMap->find( aName );
			pos = 0;
			if ( it == stringMap->end() ){
				pos = stringBuffer->size();
				*stringBuffer << aName;
				stringBuffer->write( "\0", 1 );
				stringMap->insert( make_pair( aName, pos ) );
			}else{
				pos = it->second;
			}
			out->write( pos ); //Attribute::nameOffset
			//value
			it = stringMap->find( aValue );
			pos = 0;
			if ( it == stringMap->end() ){
				pos = stringBuffer->size();
				*stringBuffer << aValue;
				stringBuffer->write( "\0", 1 );
				stringMap->insert( make_pair( aValue, pos ) );
			}else{
				pos = it->second;
			}
			out->write( pos ); //Attribute::valueOffset
		}
		//子へ
		out->write( mChildNumber ); //childNumber
		for ( int i = 0;i < mChildNumber; ++ i ){
			if ( mChildren[ i ].name() ){ //名前があるものしか出さない
				mChildren[ i ].convertToBinary( out, stringBuffer, stringMap );
			}
		}
	}
	int attributeNumber() const {
		return mAttributeNumber;
	}
	void setAttributeNumber( int n ){
		ASSERT( mAttributeNumber == 0 );
		mAttributeNumber = n;
		mAttributes = mDocument->allocateAttribute( n );
		for ( int i = 0; i < mAttributeNumber; ++i ){
			mAttributes[ i ].setDocument( mDocument );
		}
	}
	Attribute::Impl* attribute( int i ){
		ASSERT( i >= 0 && i < mAttributeNumber );
		return &mAttributes[ i ];
	}
	const Attribute::Impl* attribute( int i ) const {
		return &mAttributes[ i ];
	}
	int childNumber() const {
		return mChildNumber;
	}
	void setChildNumber( int n ){
		ASSERT( mChildNumber == 0 );
		mChildNumber = n;
		mChildren = mDocument->allocateElement( n );
		for ( int i = 0; i < mChildNumber; ++i ){
			mChildren[ i ].setDocument( mDocument );
		}
	}
	Element::Impl* child( int i ){
		return &mChildren[ i ];
	}
	const Element::Impl* child( int i ) const {
		return &mChildren[ i ];
	}
	const char* name() const {
		return mName;
	}
	void setName( const char* name ){
		mName = mDocument->allocateString( name );
	}
	void setNameReference( const char* name ){
		mName = name;
	}
	void setDocument( Document::Impl* d ){
		mDocument = d;
	}
	Document::Impl* document(){
		return mDocument;
	}
private:
	Element::Impl* mChildren;
	int mChildNumber;
	Attribute::Impl* mAttributes;
	int mAttributeNumber;
	const char* mName;
	Document::Impl* mDocument;
};

} //namespace PseudoXml
} //namespace GameLib
#endif
