#ifndef INCLUDED_GAMELIB_PSEUDOXML_TAG_H
#define INCLUDED_GAMELIB_PSEUDOXML_TAG_H

#include "GameLib/Base/Tank.h"
#include "GameLib/Base/RefString.h"
#include "GameLib/Base/Vector.h"
#include "GameLib/Base/Pool.h"
#include "PseudoXml/AttributeImpl.h"

namespace GameLib{
namespace PseudoXml{

class Attribute;

///タグをテキストから生成するクラス。
/*!
中の文字列は参照(RefString)で、自力でnewして持った物ではない。
元のテキストが存在している間しか使わないクラスという前提がある。
また、作ったAttribute::Impl*は後でElementに渡すので何も残らない。
*/
class Tag{
public:
	enum Type{
		TYPE_COMMENT,
		TYPE_BEGIN,
		TYPE_END,
		TYPE_BEGIN_END,
	};
	Tag( const char** p, const char* e, Document::Impl* document ); //これだけはcppに置く。

	Tag( const char* name ) : 
	mName( name ),
	mType( TYPE_BEGIN ),
	mDocument( 0 ){
	}
	~Tag(){
		//作ったタグは必ずElementの材料になっているはずで、
		//mAttributesはgiveAttributesで渡してしまうはずだ。
		ASSERT( mAttributes.size() == 0 );
		mDocument = 0;
	}
	const RefString* name() const {
		return &mName;
	}
	int attributeNumber() const {
		return mAttributes.size();
	}
	void addAttribute( 
	const char* name, 
	int nameSize, 
	const char* value, 
	int valueSize ){
		Attribute::Impl* a = mAttributes.add();
		a->setDocument( mDocument );
		a->setName( name, nameSize );
		if ( value ){
			a->setValue( value, valueSize );
		}
	}
	void giveAttributes( Attribute::Impl* to ){
		if ( mAttributes.size() > 0 ){
			mAttributes.copyTo( to );
			mAttributes.clear();
		}
	}
	Tag::Type type() const {
		return mType;
	}
private:
	//XML的に特殊な意味を持つ文字でないかを調べる関数
	static bool isNormalChar( char c, bool* wide ){
		unsigned char u = static_cast< unsigned char >( c );
		static const char table[ 0x100 ] = {
			0, 0, 0, 0, 0, 0, 0, 0, //0x00-0x07
			0, 0, 0, 0, 0, 0, 0, 0, //0x08-0x0f
			0, 0, 0, 0, 0, 0, 0, 0, //0x10-0x17
			0, 0, 0, 0, 0, 0, 0, 0, //0x18-0x1f
			0, 0, 0, 1, 1, 1, 0, 0, //0x20-0x27  !"#$%&'
			1, 1, 1, 1, 1, 1, 1, 0, //0x28-0x2f ()*+,-./
			1, 1, 1, 1, 1, 1, 1, 1, //0x30-0x37 01234567
			1, 1, 1, 1, 0, 0, 0, 0, //0x38-0x3f 89:;<=>?
			1, 1, 1, 1, 1, 1, 1, 1, //0x40-0x47 @ABCDEFG
			1, 1, 1, 1, 1, 1, 1, 1, //0x48-0x4f HIJKLMNO
			1, 1, 1, 1, 1, 1, 1, 1, //0x50-0x57 PQRSTUVW
			1, 1, 1, 1, 0, 1, 1, 1, //0x58-0x5f XYZ[\]^_
			1, 1, 1, 1, 1, 1, 1, 1, //0x60-0x67 `abcdefg
			1, 1, 1, 1, 1, 1, 1, 1, //0x68-0x6f hijklmno
			1, 1, 1, 1, 1, 1, 1, 1, //0x70-0x77 pqrstuvw
			1, 1, 1, 1, 1, 1, 1, 0, //0x78-0x7f xyz{|}~
			0, 2, 2, 2, 2, 2, 2, 2, //0x80-0x87 0x81-0x9f sjis 2バイト領域
			2, 2, 2, 2, 2, 2, 2, 2, //0x88-0x8f
			2, 2, 2, 2, 2, 2, 2, 2, //0x90-0x97
			2, 2, 2, 2, 2, 2, 2, 2, //0x98-0x9f
			0, 0, 0, 0, 0, 0, 0, 0, //0xa0-0xa7
			0, 0, 0, 0, 0, 0, 0, 0, //0xa8-0xaf
			0, 0, 0, 0, 0, 0, 0, 0, //0xb0-0xb7
			0, 0, 0, 0, 0, 0, 0, 0, //0xb8-0xbf
			0, 0, 0, 0, 0, 0, 0, 0, //0xc0-0xc7
			0, 0, 0, 0, 0, 0, 0, 0, //0xc8-0xcf
			0, 0, 0, 0, 0, 0, 0, 0, //0xd0-0xd7
			0, 0, 0, 0, 0, 0, 0, 0, //0xd8-0xdf
			2, 2, 2, 2, 2, 2, 2, 2, //0xe0-0xe7
			2, 2, 2, 2, 2, 2, 2, 2, //0xe8-0xef
			2, 2, 2, 2, 2, 2, 2, 2, //0xf0-0xf7
			2, 2, 2, 2, 2, 0, 0, 0, //0xf8-0xff 0xe0-0xfc sjis 2バイト領域
		};
		char t = table[ u ];
		*wide = ( ( t - 1 ) > 0 ) ? true : false;
		return ( t != 0 ) ? true : false;
	}
	Tank< Attribute::Impl > mAttributes;
	RefString mName;
	Type mType;
	Document::Impl* mDocument;
};

} //namespace PseudoXml
} //namespace GameLib 

#endif
