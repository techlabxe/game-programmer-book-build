#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Sequence/Ending.h"
#include "Sequence/Parent.h"
#include "Image.h"

namespace Sequence{

Ending::Ending() : mImage( 0 ), mCount( 0 ){
	mImage = new Image( "data/image/dummy.dds" );
}

Ending::~Ending(){
	SAFE_DELETE( mImage );
}

void Ending::update( Parent* parent ){
	if ( mCount == 120 ){ //2•b‘Ò‚Â 
		parent->moveTo( Parent::NEXT_TITLE );
	}
	//•`‰æ
	mImage->draw();
	//š‚ğo‚·
	Framework f = Framework::instance();
	f.drawDebugString( 0, 0, "µÒÃŞÄ³! ±ÅÀÊ ºÉ¹Ş°Ñ¦ ¾²Ê¼Ï¼À" );
	f.drawDebugString( 0, 1, "Â¸¯ÀËÄ Ë×ÔÏÀ¶¼" );

	++mCount;
}

} //namespace Sequence