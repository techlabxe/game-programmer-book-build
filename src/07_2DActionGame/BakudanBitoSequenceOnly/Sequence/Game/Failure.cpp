#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Sequence/Game/Failure.h"
#include "Sequence/Game/Parent.h"
#include "Image.h"
#include "State.h"

namespace Sequence{
namespace Game{

Failure::Failure() : mImage( 0 ), mCount( 0 ){
	mImage = new Image( "data/image/dummy.dds" );
}

Failure::~Failure(){
	SAFE_DELETE( mImage );
}

void Failure::update( Parent* parent ){
	if ( mCount == 60 ){ //1•b‘Ò‚Â 
		if ( parent->lifeNumber() == 0 ){
			parent->moveTo( Parent::NEXT_GAME_OVER ); 
		}else{
			parent->moveTo( Parent::NEXT_READY );
		}
	}
	//•`‰æ
	//‚Ü‚¸ƒQ[ƒ€‰æ–Ê‚ğ•`‰æ
	parent->state()->draw();
	//ã‚Éj‚¢‚ÌƒƒbƒZ[ƒW‚ğ•`‰æ
	mImage->draw();
	//ƒNƒŠƒA[‚Æš‚ğ•`‚­
	Framework::instance().drawDebugString( 0, 0, "±°¯!" );

	++mCount;
}

} //namespace Game
} //namespace Sequence
