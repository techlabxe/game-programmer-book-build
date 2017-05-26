#include "GameLib/Framework.h"
#include "File.h"


bool gFirstFrame = true;

namespace GameLib{
	void Framework::update(){
		sleep( 16 );
		if ( gFirstFrame ){
			gFirstFrame = false;
			File file( "dokaan.wav" );
			//ƒwƒbƒ_‚ğ“Ç‚İ‚à‚µ‚È‚¢‚ÅŠÛ‚²‚Æ‘—‚è‚Â‚¯‚éB
			const short* wave = reinterpret_cast< const short* >( file.getData() + 60 );
			int sampleN = ( file.getSize() - 60 ) / 2;
			playSound( wave, sampleN );
		}
	}
}