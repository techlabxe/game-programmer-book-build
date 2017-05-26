#include "GameLib/Framework.h"

namespace GameLib{
	void Framework::update(){
		unsigned* vram = videoMemory();
		static unsigned i;
		vram[ i ] += i * 100;
		i += 9973; //1–œˆÈ‰ºÅ‘å‚Ì‘f”
		i %= ( width() * height() );
	}
}
