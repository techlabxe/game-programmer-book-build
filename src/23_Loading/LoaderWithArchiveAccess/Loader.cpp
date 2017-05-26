#include "GameLib/GameLib.h"
#include "Loader.h"
#include <fstream>

#include <sstream>
#include "GameLib/Framework.h"
using namespace std;

namespace{ //無名namespaceの使用に慣れよう。
	int getInt( ifstream& in ){
		unsigned char buffer[ 4 ];
		in.read( reinterpret_cast< char* >( buffer ), 4 );
		int r = buffer[ 0 ];
		r |= ( buffer[ 1 ] << 8 );
		r |= ( buffer[ 2 ] << 16 );
		r |= ( buffer[ 3 ] << 24 );
		return r;
	}
} 

File::File( const char* filename ) : 
mFilename( filename ),
mData( 0 ),
mSize( 0 ){
}

File::~File(){
	if ( mData ){
		SAFE_DELETE_ARRAY( mData );
	}
	mSize = 0;
}

bool File::isReady() const {
	return ( mData != 0 ); //ロードが終わるまでデータに値は入らないのでこれでオーケー
}

int File::getSize() const {
	ASSERT( mData && "Loading is not finished. " );
	return mSize;
}

const char* File::getData() const {
	ASSERT( mData && "Loading is not finished. " );
	return mData;
}

Archive::Archive( const char* name ){
	//ファイルを開けてメンバに持っておく。
	mStream = new ifstream( name, ifstream::binary );
	//末尾から4バイト前へ移動
	mStream->seekg( -4, ifstream::end );
	//getInt()は4バイト読み込んでintを返す関数としよう。
	int tableBegin = getInt( *mStream );
	//テーブル先頭へ移動
	mStream->seekg( tableBegin, ifstream::beg );
	//4バイト読むとファイル数
	mFileNumber = getInt( *mStream );
	//後はループで回しながら読んでいく。
	for ( int i = 0; i < mFileNumber; ++i ){
		Entry e;
		e.mPosition = getInt( *mStream );
		e.mSize = getInt( *mStream );
		int nameLength = getInt( *mStream );
		//名前は一旦一時配列にいれる。すぐdeleteするが。
		char* name = new char[ nameLength + 1 ]; //終端NULLで+1
		mStream->read( name, nameLength );
		name[ nameLength ] = '\0'; //終端NULL
		//mEntriesはmap< char*, Entry >
		mEntries.insert( make_pair( name, e ) ); //mapに格納
		//情報を吐き出してみよう。正しいか？
//		GameLib::cout << e.mPosition << " " << e.mSize << " " << nameLength << " " << name << GameLib::endl;
		SAFE_DELETE_ARRAY( name );
	}
}

Archive::~Archive(){
	SAFE_DELETE( mStream );
}

void Archive::read( File* f )  {
	typedef map< string, Entry >::const_iterator It;
	It it = mEntries.find( f->mFilename );
	if ( it != mEntries.end() ){
		const Entry& e = it->second;
		f->mData = new char[ e.mSize ]; 
		f->mSize = e.mSize;
		//場所移動
		mStream->seekg( e.mPosition, ifstream::beg );
		//読み込み
		mStream->read( f->mData, e.mSize );
	}else{ //ない
		ASSERT( false );
	}
}

Loader* Loader::mInstance = 0;

Loader::Loader( const char* archiveName ) : mArchive( 0 ){
	if ( archiveName ){ //アーカイブがあるようです。開けて準備をするとしよう。
		mArchive = new Archive( archiveName );
	}
}

Loader::~Loader(){
	typedef list< File* >::iterator It;
	for ( It i = mFiles.begin(); i != mFiles.end(); ++i ){
		SAFE_DELETE( *i );
	}
	if ( mArchive ){
		SAFE_DELETE( mArchive );
	}
}

Loader* Loader::instance(){
	return mInstance;
}

void Loader::create( const char* archiveName ){
	ASSERT( !mInstance && "already created." );
	mInstance = new Loader( archiveName );
}

void Loader::destroy(){
	SAFE_DELETE( mInstance );
}

void Loader::createFile( File** f, const char* filename ){
	ASSERT( !( *f ) && "pointer must be NUL. " ); 
	*f = new File( filename );
	mFiles.push_back( *f );
}

void Loader::destroyFile( File** f ){
	if ( !( *f ) ){ //すでに0。やることない。
		return;
	}
	typedef list< File* >::iterator It;
	for ( It i = mFiles.begin(); i != mFiles.end(); ++i ){
		if ( *i == *f ){ //見つかった。
			SAFE_DELETE( *f ); //物を消して
			mFiles.erase( i ); //リストからも消して
			*f = 0; //受け取りポインタを0に
			break;
		}
	}
	ASSERT( !( *f ) && "can't find this pointer in the list. there must be a BUG!" );
}

void Loader::update(){
	typedef list< File* >::iterator It;
	for ( It i = mFiles.begin(); i != mFiles.end(); ++i ){
		File* f = *i;
		if ( !f->isReady() ){ //終わってねえ。ロードするよ
			if ( mArchive ){ //アーカイブから
				mArchive->read( f );
			}else{
				ifstream in( f->mFilename.c_str(), ifstream::binary );
				in.seekg( 0, ifstream::end );
				f->mSize = static_cast< int >( in.tellg() );
				in.seekg( 0, ifstream::beg );
				f->mData = new char[ f->mSize ];
				in.read( f->mData, f->mSize );
			}
		}
	}
}
