#include "XFile/Document.h"
#include "XFile/Parser.h"

namespace XFile{

Document::Document( const char* data, int size ) : mParser( 0 ){
cout.begin();
	mParser = NEW Parser( data, size );
cout.end();
}

Document::~Document(){
	SAFE_DELETE( mParser );
}

bool Document::isValid() const { 
	return mParser->isValid();
}

void Document::convertToString( string* s ) const {
	mParser->toString( s );
}

} //namespace XFile
