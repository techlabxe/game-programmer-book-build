#include "GameLib/Framework.h"

namespace GameLib{
	void Framework::update(){
		sleep( 16 );
		static int a = 0;
		if ( a++ % 120 == 0 ){
			const int F = 44100;
			short wave[ F ];
			for ( int i = 0; i < F; ++i ){
				wave[ i ] = static_cast< short >( (i*200) % 20000 );
			}
			playSound( wave, F );
		}
	}
}