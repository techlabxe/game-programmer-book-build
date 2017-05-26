#include "PseudoXml/ElementImpl.h"
#include "PseudoXml/AttributeImpl.h"

#include "GameLib/PseudoXml/Document.h"

namespace GameLib{
namespace PseudoXml{

Document::Impl::Impl() : 
mStringPool( 1024 ), //とりあえず1KBづつ
mElementPool( 16 ), //16個づつ
mAttributePool( 16 ), //16個づつ
mRoot( 0 ){
	mRoot = allocateElement( 1 );
	mRoot->setDocument( this );
}

Document::Impl::~Impl(){
}

bool Document::Impl::isReady(){
	bool r = false;
	if ( mRoot->name() ){ //名前がNULLでないならbuild済み。
		r = true;
	}else if ( mFile ){ //ロード中
		if ( mFile.isFinished() ){ //ロード終わった
			build( mFile.data(), mFile.size() );
			mFile.release();
			r = true;
		}
	}else{
		HALT( "IT MUST BE BUG!" ); //ルート要素もなく
	}
	return r;
}

void Document::Impl::build( const char* data, int size ){
	if ( RefString( data, 4 ) == "PXML" ){ //バイナリだ！
		IBinaryStream is( data, size );
		is.setPosition( 4 );
		//プールに文字列をぶっこむ
		int stringBufferOffset = is.readInt();
		int stringBufferSize = size - stringBufferOffset;
		is.setPosition( stringBufferOffset );
		char* stringBuffer = mStringPool.allocate( stringBufferSize );
		is.read( stringBuffer, stringBufferSize );
		is.setPosition( 8 );
		mRoot->setName( "" );
		mRoot->setChildNumber( is.readInt() );
		for ( int i = 0; i < mRoot->childNumber(); ++i ){
			mRoot->child( i )->build( &is, stringBuffer );
		}
	}else{ //テキストだ
		const char* begin = data;
		const char* end = begin + size;
		Tag tag( "<ROOT>" );
		mRoot->build( &tag, &begin, end );
	}
}

//単純に一個づつ呼んで行く。
void Document::Impl::convertToString( string* out ) const {
	OStringStream oss;
	int childN = mRoot->childNumber();
	for ( int i = 0; i < childN; ++i ){
		const Element::Impl* e = mRoot->child( i );
		if ( e->name() ){ //名前があるものしか出さない
			e->convertToString( &oss, 0 );
		}
	}
	Array< char > t;
	oss.get( &t );
	*out = &t[ 0 ];
}

//単純に一個づつ呼んで行く。
void Document::Impl::convertToBinary( Array< char >* out ) const {
	int childN = mRoot->childNumber();
	OStringStream binaryOut;
	OStringStream stringBuffer;
	map< RefString, int > stringMap;
	binaryOut.write( "PXML", 4  ); //マジック
	binaryOut.write( 0 ); //ストリングバッファオフセットのダミー
	binaryOut.write( childN ); //エレメント数
	for ( int i = 0; i < childN; ++i ){
		const Element::Impl* e = mRoot->child( i );
		if ( e->name() ){ //名前があるものしか出さない
			e->convertToBinary( &binaryOut, &stringBuffer, &stringMap );
		}
	}
	int stringBufferOffset = binaryOut.size();
	//ストリングバッファをマージ
	stringBuffer.get( out );
	binaryOut.write( &( *out )[ 0 ], out->size() );
	out->clear();

	binaryOut.get( out );
	//ストリングバッファオフセットの本物を入れる
	const char* p = reinterpret_cast< const char* >( &stringBufferOffset );
	for ( int i = 0; i < 4; ++i ){
		( *out )[ 4 + i ] = p[ i ];
	}
}

Element::Impl* Document::Impl::root(){
	return mRoot;
}

Element::Impl* Document::Impl::allocateElement( int n ){
	return mElementPool.allocate( n );
}

Attribute::Impl* Document::Impl::allocateAttribute( int n ){
	return mAttributePool.allocate( n );
}

} //namespace PseudoXml
} //namespace GameLib

