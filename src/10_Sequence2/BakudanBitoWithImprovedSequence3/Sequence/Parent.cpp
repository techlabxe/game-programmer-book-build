#include "GameLib/GameLib.h"
#include "Sequence/Parent.h"
#include "Sequence/Title.h"

namespace Sequence{

Parent* Parent::mInstance = 0;

void Parent::create(){
	ASSERT( !mInstance );
	mInstance = new Parent();
}

void Parent::destroy(){
	ASSERT( mInstance );
	SAFE_DELETE( mInstance );
}

Parent* Parent::instance(){
	return mInstance;
}

Parent::Parent() : 
mMode( MODE_NONE ),
mChild( 0 ){
	//Å‰‚Éì‚é‚Ì‚Íƒ^ƒCƒgƒ‹
	mChild = new Title();
}

Parent::~Parent(){
	//c‚Á‚Ä‚¢‚ê‚Î–•E
	SAFE_DELETE( mChild );
}

void Parent::update(){
	Base* nextChild = mChild->update( this );
	if ( nextChild != mChild ){
		//‘JˆÚ”»’è
		Child* casted = dynamic_cast< Child* >( nextChild );
		ASSERT( casted ); //¸”s‚Í‚ ‚è‚¦‚È‚¢B
		SAFE_DELETE( mChild );
		mChild = casted;
	}
	nextChild = 0; //”O‚Ì‚½‚ß
}

void Parent::setMode( Mode mode ){
	mMode = mode;
}

Parent::Mode Parent::mode() const {
	return mMode;
}

} //namespace Sequence
