#include "GameLib/GameLib.h"
#include "PseudoXml/Document.h"
#include "PseudoXml/Tag.h"
#include "PseudoXml/Element.h"
#include "Library/File.h"

namespace PseudoXml{

Document::Document() : mRoot( 0 ){
	mRoot = new Element( "<ROOT>" );
}

Document::Document( const char* filename ) : mRoot( 0 ){
	//ファイルを全部読み込み
	File file( filename );
	//ダミーの根元エレメント用にタグを用意。名前は<ROOT>
	//<は名前には使えない文字なので、これが特別であることがすぐわかる。
	Tag tag( "<ROOT>" );
	const char* begin = file.data();
	const char* end = begin + file.size();
	mRoot = new Element( &tag, &begin, end );
}

Document::~Document(){
	SAFE_DELETE( mRoot );
}

void Document::write( const char* filename ) const {
	string str;
	//ルート要素は無視してかきこみ
	for ( int i = 0; i < mRoot->childNumber(); ++i ){
		mRoot->child( i )->convertToString( &str, 0 );
	}
	File::write( filename, str.c_str(), static_cast< int >( str.size() ) );
}

const Element* Document::root() const {
	return mRoot;
}

Element* Document::root(){
	return mRoot;
}

} //namespace PseudoXml
