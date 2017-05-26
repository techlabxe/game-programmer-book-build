#include "GameLib/Framework.h"

namespace GameLib{
	void Framework::update(){
		sleep( 16 );
		static int a = 0;
		static int b = 0;
		if ( a++ % 120 == 0 ){
			const int N = 44100;
			short wave[ N ];
			int f = ( b % 2 == 0 ) ? 200 : 100;
			int m = ( b % 2 == 0 ) ? 20000 : 10000;
			for ( int i = 0; i < N; ++i ){
				wave[ i ] = static_cast< short >( (i*f) % m );
			}
			playSound( wave, N );
			++b;
		}
	}
}
