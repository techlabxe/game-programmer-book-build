#ifndef INCLUDED_XFILE_DOCUMENT_H
#define INCLUDED_XFILE_DOCUMENT_H

#include <string>

namespace XFile{

class Parser;

//XFileクラス
/*!
XFileからデータ構造を構築する。ただし構築したデータ構造へは直接アクセスはできず、
xmlにしてしまってからxmlパーサで読み取ることになる。
これは、XFileを直接のモデルフォーマットとして使わず、
単なる中間フォーマットとしか見ていないからである。
*/
class Document{
public:
	Document( const char* data, int size );
	~Document();
	bool isValid() const;
	///xml文字列に変換。これ以外のことはできない。
	void convertToString( std::string* ) const;
private:
	Parser* mParser;
};

} //namespace XFile

#endif
