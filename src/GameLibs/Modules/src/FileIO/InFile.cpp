#include "GameLib/GameLib.h"
#include "GameLib/FileIO/InFile.h"
#include "FileIO/InFileImpl.h"
#include "FileIO/ManagerImpl.h"

namespace GameLib{
namespace FileIO{

InFile InFile::create( const char* filename ){
	InFile r;
	r.mImpl = gManagerImpl->createInFile( filename );
	return r;
}

void InFile::release(){
	if ( mImpl ){
		gManagerImpl->destroyInFile( mImpl );
	}
	mImpl = 0;
}

int InFile::size() const {
	ASSERT( mImpl && "FileIO::InFile : This is empty object." );
	return mImpl->size();
}

const char* InFile::data() const {
	ASSERT( mImpl && "FileIO::InFile : This is empty object." );
	return mImpl->data();
}

bool InFile::isFinished() const {
	ASSERT( mImpl && "FileIO::InFile : This is empty object." );
	return mImpl->isFinished();
}

bool InFile::isError() const {
	ASSERT( mImpl && "FileIO::InFile : This is empty object." );
	return mImpl->isError();
}

int InFile::getInt( int p ) const {
	ASSERT( mImpl && "FileIO::InFile : This is empty object." );
	return static_cast< int >( mImpl->getUnsigned( p ) );
}

unsigned InFile::getUnsigned( int p ) const {
	ASSERT( mImpl && "FileIO::InFile : This is empty object." );
	return mImpl->getUnsigned( p );
}

short InFile::getShort( int p ) const {
	ASSERT( mImpl && "FileIO::InFile : This is empty object." );
	return static_cast< short >( mImpl->getUnsignedShort( p ) ); 
}

unsigned short InFile::getUnsignedShort( int p ) const {
	ASSERT( mImpl && "FileIO::InFile : This is empty object." );
	return mImpl->getUnsignedShort( p );
}

//ˆÈ‰º’è”ÔƒR[ƒh
InFile::InFile() : mImpl( 0 ){
}

InFile::InFile( const InFile& o ) : mImpl( o.mImpl ){
	if ( mImpl ){
		mImpl->refer();
	}
}

InFile::~InFile(){
	release();
}

InFile& InFile::operator=( const InFile& o ){
	release();
	mImpl = o.mImpl;
	if ( mImpl ){
		mImpl->refer();
	}
	return *this;
}

bool InFile::operator==( const InFile& o ) const {
	return ( mImpl == o.mImpl );
}

bool InFile::operator!=( const InFile& o ) const {
	return ( mImpl != o.mImpl );
}

InFile::operator void*() const {
	return ( mImpl == 0 ) ? 0 : reinterpret_cast< void* >( ~0 );
}

} //namespace FileIO
} //namespace GameLib
