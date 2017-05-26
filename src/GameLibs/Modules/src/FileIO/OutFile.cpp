#include "GameLib/GameLib.h"
#include "GameLib/FileIO/OutFile.h"
#include "FileIO/OutFileImpl.h"
#include "FileIO/ManagerImpl.h"

namespace GameLib{
namespace FileIO{

OutFile OutFile::create(
const char* filename,
const char* data,
int size ){
	OutFile r;
	r.mImpl = gManagerImpl->createOutFile( filename, data, size );
	return r;
}

void OutFile::release(){
	if ( mImpl ){
		mImpl->release();
		if ( mImpl->referenceCount() == 0 ){
			gManagerImpl->destroyOutFile( mImpl );
		}
	}
	mImpl = 0;
}

bool OutFile::isFinished() const {
	ASSERT( mImpl && "FileIO::OutFile : This is empty object." );
	return mImpl->isFinished();
}

bool OutFile::isError() const {
	ASSERT( mImpl && "FileIO::OutFile : This is empty object." );
	return mImpl->isError();
}

//ˆÈ‰º’è”ÔƒR[ƒh
OutFile::OutFile() : mImpl( 0 ){
}

OutFile::OutFile( const OutFile& o ) : mImpl( o.mImpl ){
	if ( mImpl ){
		mImpl->refer();
	}
}

OutFile::~OutFile(){
	release();
}

OutFile& OutFile::operator=( const OutFile& o ){
	release();
	mImpl = o.mImpl;
	if ( mImpl ){
		mImpl->refer();
	}
	return *this;
}

bool OutFile::operator==( const OutFile& o ) const {
	return ( mImpl == o.mImpl );
}

bool OutFile::operator!=( const OutFile& o ) const {
	return ( mImpl != o.mImpl );
}

OutFile::operator void*() const {
	return ( mImpl == 0 ) ? 0 : reinterpret_cast< void* >( ~0 );
}

} //namespace FileIO
} //namespace GameLib
