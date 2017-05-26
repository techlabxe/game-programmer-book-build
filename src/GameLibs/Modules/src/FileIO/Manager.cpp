#include "GameLib/GameLib.h"
#include "GameLib/FileIO/Manager.h"
#include "FileIO/ManagerImpl.h"
#include <string>
#include <fstream>

namespace GameLib{
namespace FileIO{
using namespace std;

ManagerImpl* gManagerImpl = 0;

void Manager::create(
const char** archiveNames,
int archiveNumber,
AccessMode am ){
	STRONG_ASSERT( !gManagerImpl );
	gManagerImpl = NEW ManagerImpl( archiveNames, archiveNumber, am );
}

void Manager::destroy(){
	SAFE_DELETE( gManagerImpl );
}

Manager Manager::instance(){
	ASSERT( gManagerImpl && "FileIO Module hasn't started yet!" );
	return Manager();
}

void Manager::write( 
const char* filename,
const char* data,
int size ){
	gManagerImpl->writeFile( filename, data, size );
}

string Manager::createListString() const {
	return gManagerImpl->createListString();
}

void Manager::setLimit( int warning, int error ){
	gManagerImpl->setLimit( warning, error );
}

void Manager::enableHaltOnError( bool f ){
	gManagerImpl->enableHaltOnError( f );
}

} //namespace Sound
} //namespace GameLib