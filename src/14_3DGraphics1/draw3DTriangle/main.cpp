#include "GameLib/Framework.h"

int gCount;

namespace GameLib{
	void Framework::update(){
		setFrameRate( 60 );
		double p0[ 3 ] = { 100.0, 100.0, 0.0 };
		double p1[ 3 ] = { 400.0, 200.0, 0.4 };
		double p2[ 3 ] = { 200.0, 400.0, 0.8 };

		double zOffset = ( gCount % 200 ) * 0.01 - 1.0;
		p0[ 2 ] += zOffset;
		p1[ 2 ] += zOffset;
		p2[ 2 ] += zOffset;

		drawTriangle3D( p0, p1, p2 );

		++gCount;
	}
}
