#include "GameLib/Framework.h"

namespace GameLib{
	void Framework::update(){
		unsigned* vram = videoMemory();
		static int color = 1;
		int pixelNumber = width() * height();
		for ( int i = 0; i < pixelNumber; ++i ){
			vram[ i ] = color;
		}
		color *= 3;
	}
}