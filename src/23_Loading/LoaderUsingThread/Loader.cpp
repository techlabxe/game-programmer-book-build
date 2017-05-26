#include "GameLib/GameLib.h"
#include "GameLib/Threading/Functions.h"
#include "Loader.h"
#include <fstream>
using namespace std;
using namespace GameLib;
using namespace GameLib::Threading;

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

void LoadingThread::operator()(){
	bool end = false;
	while ( !end ){ //無限ループ
		Threading::sleep( 1 ); //一周ごとにしばらく寝る。グルグル回るのを防ぎたい。
		Loader::instance()->update( &end );
	}
}

Loader* Loader::mInstance = 0;

Loader::Loader() : mThread( 0 ), mEndRequest( false ){
	mThread = new LoadingThread;
	mLock = Mutex::create();

	//0うめ
	for ( int i = 0; i < MAX_FILE_NUMBER; ++i ){
		mFiles[ i ] = 0;
	}
}

Loader::~Loader(){
	//スレッドを止める
	mLock.lock();
	mEndRequest = true;
	mLock.unlock();
	//スレッドをdelete。中でwait()して終了を待つ
	SAFE_DELETE( mThread );

	//リストを抹殺
	for ( int i = 0; i < MAX_FILE_NUMBER; ++i ){
		if ( mFiles[ i ] ){
			SAFE_DELETE( mFiles[ i ] );
		}
	}
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
	//空いてる場所を探す
	for ( int i = 0; i < MAX_FILE_NUMBER; ++i ){
		if ( !mFiles[ i ] ){
			*f = new File( filename );
			mLock.lock(); //足す前に入る
			mFiles[ i ] = *f;
			mLock.unlock();
			break;
		}
	}
}

void Loader::destroyFile( File** f ){
	if ( !( *f ) ){ //すでに0。やることない。
		return;
	}
	//探す
	for ( int i = 0; i < MAX_FILE_NUMBER; ++i ){
		if ( mFiles[ i ] == *f ){
			mLock.lock(); //消す前に入る
			SAFE_DELETE( mFiles[ i ] );
			mLock.unlock();
			*f = 0; //受け取りポインタを0に
			break;
		}
	}
	ASSERT( !( *f ) && "can't find this pointer in the list. there must be a BUG!" );
}

void Loader::update( bool* endOut ){
	for ( int i = 0; i < MAX_FILE_NUMBER; ++i ){
		string filename; //ファイル名を取り出すために。

		mLock.lock();
		if ( mEndRequest ){ //終われと言われている。次のループで抜けます。
			*endOut = true;
		}
		if ( mFiles[ i ] && !mFiles[ i ]->isReady() ){
			filename = mFiles[ i ]->mFilename;
		}
		mLock.unlock(); //ロード終了かどうかだけ見て一回出る

		if ( filename.size() > 0 ){ //ファイル名が入ってる。ロードしろということだ。
			int size;
			char* data = 0;
			//一旦ローカルの変数にロードする。そうすればクリティカルセクションに入らずに済む。
			ifstream in( filename.c_str(), ifstream::binary );
			in.seekg( 0, ifstream::end );
			size = static_cast< int >( in.tellg() ); 
			in.seekg( 0, ifstream::beg );
			data = new char[ size ];
			in.read( data, size );
			//ロードが終わってからクリティカルセクションに入る。入りっぱなしだと激しく性能が落ちる。
			mLock.lock();
			if ( mFiles[ i ] ){ //もうないかもしれない。チェック。
				mFiles[ i ]->mData = data;
				data = 0; //管理権を渡したのだから0に。
				mFiles[ i ]->mSize = size;
			}
			mLock.unlock();
			if ( data ){ //もしデータがまだあるということは、上のifに入れなかったということだから破棄。
				//deleteは遅いのでクリティカルセクションに入れたくなかったのだ。
				SAFE_DELETE_ARRAY( data );
			}
		}
	}
}
