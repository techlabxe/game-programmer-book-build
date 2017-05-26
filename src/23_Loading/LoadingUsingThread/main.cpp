#include "GameLib/Framework.h"
#include "GameLib/Threading/Thread.h"
#include <sstream>
#include <fstream>
using namespace std;
using namespace GameLib;
using namespace GameLib::Threading;

class LoadingThread : public Thread{
public:
	LoadingThread( const char* filename ) :
	mFilename( filename ),
	mData( 0 ),
	mSize( 0 ),
	mFinished( false ){
		start();
	}
	~LoadingThread(){
		wait(); //待ちます
		if ( mData ){
			SAFE_DELETE_ARRAY( mData );
		}
	}
	void operator()(){
		ifstream in( mFilename.c_str(), ifstream::binary );
		in.seekg( 0, ifstream::end );
		mSize = static_cast< int >( in.tellg() );
		in.seekg( 0, ifstream::beg );
		mData = new char[ mSize ];
		in.read( mData, mSize );
		mFinished = true;
	}
	string mFilename;
	char* mData;
	int mSize;
	bool mFinished;
};

LoadingThread* gThread;
int gCount;

namespace GameLib{
	void Framework::update(){
		//ロード開始
		if ( !gThread ){
			gThread = new LoadingThread( "../data/robo.dds" );
			cout << "start loading!" << endl;
		}
		//ロード終わった！
		if ( gThread->mFinished ){
			SAFE_DELETE( gThread );
			cout << "loading finished!" << endl;
		}

		//描画
		ostringstream oss;
		int framerate = frameRate();
		oss << "Framerate: " << frameRate();
		int c = ( framerate > 255 ) ? 255 : framerate;
		drawDebugString( 0, 0, oss.str().c_str(), 0xffff0000 | ( c << 8 ) | c );
		int previousFrame = previousFrameInterval();
		oss.str( "" );
		oss << "PreviousFrame: " << previousFrameInterval();
		c = ( previousFrame > 63 ) ? 0 : ( 255 - previousFrame * 4 );
		drawDebugString( 0, 1, oss.str().c_str(), 0xffff0000 | ( c << 8 ) | c );

		//止まってるのをわかりやすくする描画
		double p[ 4 ][ 4 ];
		p[ 0 ][ 2 ] = p[ 1 ][ 2 ] = p[ 2 ][ 2 ] = p[ 3 ][ 2 ] = 0.0;
		p[ 0 ][ 3 ] = p[ 1 ][ 3 ] = p[ 2 ][ 3 ] = p[ 3 ][ 3 ] = 1.0;
		p[ 0 ][ 0 ] = p[ 2 ][ 0 ] = ( gCount % 640 ) / 320.0 - 1.05;
		p[ 1 ][ 0 ] = p[ 3 ][ 0 ] = ( gCount % 640 ) / 320.0 - 0.95;
		p[ 0 ][ 1 ] = p[ 1 ][ 1 ] = -0.05;
		p[ 2 ][ 1 ] = p[ 3 ][ 1 ] = 0.05;
		drawTriangle3DH( p[ 0 ], p[ 1 ], p[ 2 ] );
		drawTriangle3DH( p[ 3 ], p[ 1 ], p[ 2 ] );
		++gCount;

		if ( isEndRequested() ){
			SAFE_DELETE( gThread );
		}
	}
} //namespace GameLib