#ifndef INCLUDED_GAMELIB_FILEIO_OUTFILEIMPL_H
#define INCLUDED_GAMELIB_FILEIO_OUTFILEIMPL_H

#include "GameLib/GameLib.h"
#include "GameLib/FileIO/OutFile.h"
#include "GameLib/Base/Impl/ReferenceType.h"
#include <fstream>
#include <list>
#include <string>

namespace GameLib{
namespace FileIO{
using namespace std;

class OutFile::Impl : public ReferenceType{
public:
	typedef list< Impl* >::iterator It;
	Impl( int id ) : 
	mId( id ),
	mIsError( false ),
	mIsFinished( false ){
	}
	~Impl(){
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
	void setFinished(){
		mIsFinished = true;
	}
	int id() const {
		return mId;
	}
private:
	int mId;
	bool mIsError;
	bool mIsFinished;
};

} //namespace FileIO
} //namespace GameLib

#endif
