#ifndef INCLUDED_GAMELIB_PSEUDOXML_DOCUMENT_H
#define INCLUDED_GAMELIB_PSEUDOXML_DOCUMENT_H

#include <string>

namespace GameLib{
template< class T > class Array;
namespace PseudoXml{
using namespace std;

class Element;
class ConstElement;

///ファイルアクセス機能を持つニセxml根元クラス
class Document{
public:
	///書き込み用に空で生成
	static Document create();
	///ファイルからコンストラクト(要FileIO初期化)
	static Document create( const char* filename );
	///ロード済み文字列からコンストラクト
	static Document create( const char* data, int size );
	///根Elementゲット
	Element root();
	///根Elementゲット
	ConstElement root() const; 
	///ロード済みか否か
	bool isReady();
	///読み込み中エラーが発生したか
	bool isError() const;
	///ニセxmlテキストを生成
	void convertToString( string* out ) const;
	///ニセxmlバイナリを生成
	void convertToBinary( Array< char >* out ) const;
	///ニセxmlファイルを生成(要FileIO初期化)
	void write( const char* filename, bool isBinary = false ) const;
	///子の数ゲット
	int childNumber() const;
	///子ゲット
	Element child( int i );
	///子ゲット
	ConstElement child( int i ) const;

	//以下意識しなくていい
	Document();
	Document( const Document& );
	~Document();
	operator void*() const;
	Document& operator=( const Document& );
	void release();
	bool operator==( const Document& ) const;
	bool operator!=( const Document& ) const;

	class Impl;
private:
	Impl* mImpl;
};

//const版
class ConstDocument{
public:
	///根Elementゲット
	ConstElement root() const; 
	///読み込み中エラーが発生したか
	bool isError() const;
	///ニセxmlテキストを生成
	void convertToString( string* out ) const;
	///ニセxmlバイナリを生成
	void convertToBinary( Array< char >* out ) const;
	///ニセxmlファイルを生成(要FileIO初期化)
	void write( const char* filename, bool isBinary = false ) const;
	///子の数ゲット
	int childNumber() const;
	///子ゲット
	ConstElement child( int i ) const;

	//以下知らなくていい
	ConstDocument();
	ConstDocument( const Document& );
	~ConstDocument();
	ConstDocument& operator=( const Document& );
	void release();
	operator void*() const;
	bool operator==( const Document& ) const;
	bool operator!=( const Document& ) const;
private:
	Document mBase;
};

} //namespace PseudoXml
} //namespace GameLib

#endif
