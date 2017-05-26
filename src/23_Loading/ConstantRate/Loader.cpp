#include "GameLib/GameLib.h"
#include "Loader.h"
#include <fstream>
using namespace std;

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

Loader* Loader::mInstance = 0;

Loader::Loader() : 
mCurrentFile( 0 ),
mStream( 0 ),
mFileSize( 0 ),
mFilePosition( 0 ),
mData( 0 ){
}

Loader::~Loader(){
	typedef list< File* >::iterator It;
	for ( It i = mFiles.begin(); i != mFiles.end(); ++i ){
		SAFE_DELETE( *i );
	}
	//読んでるファイルがあるなら破棄
	if ( mStream ){
		SAFE_DELETE( mStream );
	}
	if ( mData ){
		SAFE_DELETE( mData );
	}
	mCurrentFile = 0; //これはdeleteしない。リストの中にいるはずだ
}

Loader* Loader::instance(){
	return mInstance;
}

void Loader::create(){
	ASSERT( !mInstance && "already created." );
	mInstance = new Loader();
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
	//すでにロード中ならロードを進める
	if ( mCurrentFile ){
		//でも、これ本当にリストにあるの？
		bool found = false;
		for ( It i = mFiles.begin(); i != mFiles.end(); ++i ){
			if ( *i == mCurrentFile ){
				found = true; //あった
				break;
			}
		}
		if ( !found ){ //もうないじゃないか。すてちゃう。途中でdestroy()されたようだ。
			if ( mData ){
				SAFE_DELETE_ARRAY( mData );
			}
			if ( mStream ){
				SAFE_DELETE( mStream );
			}
			mFileSize = mFilePosition = 0;
			mCurrentFile = 0;
		}else{ //ファイルがある。じゃあロードを続けるか。
			int rest = mFileSize - mFilePosition;
			//今回読むサイズはREAD_UNIT以上ならREAD_UNIT、以下ならrestそのまま
			int size = ( rest > READ_UNIT ) ? READ_UNIT : rest;
			//読み込み
			mStream->read( mData + mFilePosition, size );
			mFilePosition += size;
			//終わったならセットしよう。
			if ( size == rest ){ //今回の読み込み量と残りが等しい=終わった
				mCurrentFile->mData = mData;
				mCurrentFile->mSize = mFileSize;
				mData = 0; //肩の荷が下りたので0に
				mFileSize = mFilePosition = 0;
				SAFE_DELETE( mStream );
				mCurrentFile = 0;
			}
		}
	}else{ //さて、読み込み中のものがないならファイルを開けねばならないなあ。
		for ( It i = mFiles.begin(); i != mFiles.end(); ++i ){
			File* f = *i;
			if ( !f->isReady() ){ //終わってねえ。ロードするよ
				mStream = new ifstream( f->mFilename.c_str(), ifstream::binary );
				mStream->seekg( 0, ifstream::end );
				mFileSize = static_cast< int >( mStream->tellg() );
				mData = new char[ mFileSize ];
				mStream->seekg( 0, ifstream::beg ); //最初に巻き戻しておく。
				mFilePosition = 0;
				mCurrentFile = f;
				break; //抜ける。
			}
		}
	}
}
