#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Sequence/Game/Failure.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/Game/Ready.h"
#include "Image.h"

namespace Sequence{
namespace Game{

Failure::Failure() : mImage( 0 ), mCount( 0 ){
	mImage = new Image( "data/image/dummy.dds" );
}

Failure::~Failure(){
	SAFE_DELETE( mImage );
}

Child* Failure::update( Parent* parent ){
	Child* next = this;
	if ( mCount == 60 ){ //1•b‘Ò‚Â 
		if ( parent->lifeNumber() == 0 ){
			parent->moveTo( Parent::NEXT_GAME_OVER ); 
		}else{
			next = new Ready;
		}
	}
	//•`‰æ
	//‚Ü‚¸ƒQ[ƒ€‰æ–Ê‚ğ•`‰æ
	parent->drawState();
	//ã‚Éj‚¢‚ÌƒƒbƒZ[ƒW‚ğ•`‰æ
	mImage->draw();
	//ƒNƒŠƒA[‚Æš‚ğ•`‚­
	Framework::instance().drawDebugString( 0, 0, "±°¯!" );

	++mCount;

	return next;
}

} //namespace Game
} //namespace Sequence
