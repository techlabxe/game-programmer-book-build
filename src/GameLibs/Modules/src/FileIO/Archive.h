#ifndef INCLUDED_GAMELIB_FILEIO_ARCHIVE_H
#define INCLUDED_GAMELIB_FILEIO_ARCHIVE_H

#include "GameLib/Base/HashMap.h"
#include "GameLib/base/Compressor.h"
#include <fstream>

namespace GameLib{
namespace FileIO{
using namespace std;

class Archive{
public:
	Archive( const char* name ) : 
	mFileNumber( 0 ),
	mBlockSize( 0 ),
	mStream( 0 ),
	mDummy( false ){
		if ( !name ){
			mDummy = true; //ダミーアーカイブ。直接ファイルから読み込みます。
			return;
		}
		//ファイルを開けてメンバに持っておく。
		setlocale( LC_ALL, "" ); //これがないと日本語ファイル名を受け付けない
		mStream = NEW ifstream( name, ifstream::binary );
		if ( !mStream->good() ){
			cout << "can't open archive: " << name << endl;
			SAFE_DELETE( mStream );
			return;
		}
		//末尾から4バイト前へ移動
		//ファイルサイズを一回測ろうか。エラーチェックのためにな。
		mStream->seekg( 0, ifstream::end );
		streamsize fileSize = mStream->tellg();
		if ( fileSize < 8 ){ //まずありえん
			cout << "archive size is illegal ( too small ): " << name << endl;
			SAFE_DELETE( mStream );
			return;
		}
		mStream->seekg( -8, ifstream::end );
		mBlockSize = getUnsigned();
		streamsize tableBegin = getUnsigned() * mBlockSize;
		//テーブル先頭へ移動
		if ( tableBegin + 12 >= fileSize ){ //ありえん
			cout << "archive size is illegal ( wrong table ): " << name << endl;
			SAFE_DELETE( mStream );
			return;
		}
		mStream->seekg( tableBegin, ifstream::beg );
		//4バイト読むとファイル数
		mFileNumber = getUnsigned();
		if ( fileSize < mFileNumber * 16 ){ //ありえん
			cout << "archive size is illegal ( wrong file number ): " << name << endl;
			SAFE_DELETE( mStream );
			return;
		}
		//ハッシュマップサイズ確保
		mEntries.setCapacity( mFileNumber );
		//後はループで回しながら読んでいく。
		for ( int i = 0; i < mFileNumber; ++i ){
			Entry e;
			e.mPosition = getUnsigned();
			e.mSize = getUnsigned();
			e.mOriginalSize = getUnsigned();
			e.mNecessaryBufferSize = getUnsigned();
			int nameLength = getUnsigned();
			//名前は一旦一時配列にいれる。すぐdeleteするが。
			char* name = NEW char[ nameLength + 1 ]; //終端NULLで+1
			mStream->read( name, nameLength );
			name[ nameLength ] = '\0'; //終端NULL
			//mEntriesはmap< char*, Entry >
			mEntries.add( name, e ); //mapに格納
			//情報を吐き出してみよう。正しいか？
			SAFE_DELETE_ARRAY( name );
		}
	}
	~Archive(){
		SAFE_DELETE( mStream );
	}
	void open( 
	ifstream** streamOut,
	int* entryIndex,
	const char* name ){
		if ( mDummy ){
			*entryIndex = -1;
			setlocale( LC_ALL, "" ); //これがないと日本語ファイル名を受け付けない
			*streamOut = NEW ifstream( name, ifstream::binary );
			if ( !( ( *streamOut )->good() ) ){
				SAFE_DELETE( *streamOut ); //ない
			}
		}else{
			if ( mFileNumber == 0 ){ //ないアーカイブなら常に見つからない
				*entryIndex = -1;
				*streamOut = 0;
			}else{
				//スラッシュとバックスラッシュの両方を受け付けるためにバックスラッシュ化
				string tName = name;
				for ( size_t i = 0; i < tName.size(); ++i ){
					if ( tName[ i ] == '/' ){
						tName[ i ] = '\\';
					}
				}
				int it = mEntries.find( tName );
				if ( !mEntries.isEnd( it ) ){
					*entryIndex = it;
					*streamOut = mStream;
					const Entry& e = *mEntries.value( *entryIndex );
					streamsize pos = e.mPosition * mBlockSize;
					mStream->seekg( pos, ifstream::beg );
				}else{
					*entryIndex = -1;
					*streamOut = 0;
				}
			}
		}
	}
	void getFileSize( streamsize* readSize, streamsize* originalSize, int entryIndex, ifstream* stream ) const {
		if ( mDummy ){
			stream->seekg( 0, ifstream::end );
			streamsize r = stream->tellg();
			stream->seekg( 0, ifstream::beg );
			*readSize = *originalSize = r;
		}else{
			STRONG_ASSERT( entryIndex >= 0 );
			STRONG_ASSERT( stream == mStream );
			const Entry& e = *mEntries.value( entryIndex );
			*readSize = e.mSize;
			*originalSize = e.mOriginalSize;
		}
	}
	void allocate( char** dataOut, int size, int entryIndex ){
		if ( mDummy ){
			STRONG_ASSERT( entryIndex == -1 );
			*dataOut = NEW char[ size + 1 ]; //親切設計NULL終端。
		}else{
			STRONG_ASSERT( entryIndex >= 0 );
			const Entry& e = *mEntries.value( entryIndex );
			int allocSize = max( e.mNecessaryBufferSize, e.mOriginalSize + 1 ); //展開に必要なバッファサイズがあるならそれも書く。
			*dataOut = NEW char[ allocSize ];
		}
	}
	void read(
	bool* errorOut,
	char* data,
	int size,
	int entryIndex, 
	ifstream* stream ){
		bool compressed = false;
		int originalSize = size;
		int necessaryBufferSize = size;
		if ( !mDummy ){
			STRONG_ASSERT( stream == mStream );
			STRONG_ASSERT( entryIndex >= 0 );
			const Entry& e = *mEntries.value( entryIndex );
			STRONG_ASSERT( e.mSize == size );
			if ( e.mOriginalSize != size ){
				compressed = true;
				originalSize = e.mOriginalSize;
				necessaryBufferSize = e.mNecessaryBufferSize;
			}
		}
		int readOffset = 0;
		if ( compressed ){
			readOffset = necessaryBufferSize - size; //後ろにロード。展開に余分に必要な場合があるのでoriginalSizeではない。
		}
		stream->read( data + readOffset, size );
		if ( stream->gcount() != size ){ //エラーか
			*errorOut = true;
			return;
		}
		if ( compressed ){
			int outSize;
			Compressor::decompress(
				data,
				&outSize,
				data + readOffset,
				size );
			if ( outSize != originalSize ){
				cout << "Decompression Failed!" << endl;
				*errorOut = true;
			}
		}
	}
	void close(	ifstream** stream ){
		if ( mDummy ){
			SAFE_DELETE( *stream );
		}else{
			*stream = 0;
		}
	}
private:
	class Entry{
	public:
		unsigned mPosition;
		int mSize;
		int mOriginalSize;
		int mNecessaryBufferSize;
	};
	unsigned getUnsigned(){
		unsigned char buffer[ 4 ];
		mStream->read( reinterpret_cast< char* >( buffer ), 4 );
		unsigned r = buffer[ 0 ];
		r |= ( buffer[ 1 ] << 8 );
		r |= ( buffer[ 2 ] << 16 );
		r |= ( buffer[ 3 ] << 24 );
		return r;
	}
	int mFileNumber;
	unsigned mBlockSize;
	ifstream* mStream;
	HashMap< string, Entry > mEntries;
	bool mDummy;
};

} //namespace FileIO
} //namespace GameLib

#endif
