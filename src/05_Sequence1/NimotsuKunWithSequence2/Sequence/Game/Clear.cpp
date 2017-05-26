#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Sequence/Game/Clear.h"
#include "Sequence/Game/Parent.h"
#include "Image.h"
#include "State.h"

namespace Sequence{
namespace Game{

Clear::Clear() : mImage( 0 ), mCount( 0 ){
	mImage = new Image( "data/image/clear.dds" );
}

Clear::~Clear(){
	SAFE_DELETE( mImage );
}

void Clear::update( Parent* parent ){
	if ( mCount == 60 ){ //60ƒtƒŒ[ƒ€‚ÅˆÚ“®
		parent->moveTo( Parent::SEQ_STAGE_SELECT );
	}

	//•`‰æ
	//‚Ü‚¸ƒQ[ƒ€‰æ–Ê‚ð•`‰æ
	parent->state()->draw();
	//ã‚Éj‚¢‚ÌƒƒbƒZ[ƒW‚ð•`‰æ
	mImage->draw();

	++mCount;
}

} //namespace Game
} //namespace Sequence
