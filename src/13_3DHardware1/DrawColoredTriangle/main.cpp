#include "GameLib/Framework.h"

namespace GameLib{
	void Framework::update(){
		double p0[ 2 ] = { 100.0, 100.0 };
		double p1[ 2 ] = { 200.0, 120.0 };
		double p2[ 2 ] = { 120.0, 200.0 };
		drawTriangle2D( p0, p1, p2, 0, 0, 0, 0xffff0000, 0xffffffff, 0xff0000ff );
	}
}
