#ifndef INCLUDED_GAMELIB_FILEIO_INFILEIMPL_H
#define INCLUDED_GAMELIB_FILEIO_INFILEIMPL_H

#include "GameLib/GameLib.h"
#include "GameLib/FileIO/InFile.h"
#include "GameLib/Base/Impl/ReferenceType.h"
#include <fstream>
#include <map>
#include <string>

namespace GameLib{
namespace FileIO{
using namespace std;

class InFile::Impl : public ReferenceType{
public:
	typedef map< string, Impl* >::iterator It;
	Impl() : 
	mData( 0 ),
	mSize( 0 ),
	mIsError( false ),
	mIsFinished( false ){
	}
	~Impl(){
		SAFE_DELETE_ARRAY( mData );
		mSize = 0;
	}
	int size() const {
		return mSize;
	}
	const char* data() const {
		return mData;
	}
	bool isFinished() const {
		return mIsFinished;
	}
	bool isError() const {
		return mIsError;
	}
	void setError(){ //ÉGÉâÅ[Ç»ÇÁèIÇÌÇËÇ≈Ç†ÇÈ
		mIsError = mIsFinished = true;
	}
	void setIterator( It it ){
		mIterator = it;
	}
	It iterator() const {
		return mIterator;
	}
	void set( char* data, int size ){
		ASSERT( !mIsFinished );
		mData = data;
		mSize = size;
		if ( !mData ){
			mIsError = true;
		}
		mIsFinished = true;
	}
	unsigned getUnsigned( int p ) const {
		ASSERT( mIsFinished && !mIsError );
		const unsigned char* up;
		up = reinterpret_cast< const unsigned char* >( mData );
		unsigned r = up[ p + 0 ];
		r |= ( up[ p + 1 ] << 8 ); 
		r |= ( up[ p + 2 ] << 16 ); 
		r |= ( up[ p + 3 ] << 24 ); 
		return r;
	}
	unsigned short getUnsignedShort( int p ) const {
		ASSERT( mIsFinished && !mIsError );
		const unsigned char* up;
		up = reinterpret_cast< const unsigned char* >( mData );
		unsigned short r = up[ p + 0 ];
		r |= ( up[ p + 1 ] << 8 ); 
		return r;
	}
private:
	char* mData;
	int mSize;
	bool mIsError;
	bool mIsFinished;
	It mIterator;
};

} //namespace FileIO
} //namespace GameLib

#endif
