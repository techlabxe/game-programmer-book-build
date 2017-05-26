#include "GameLib/Framework.h"
using namespace GameLib;
#include "Image.h"

Image* gImage;
Texture* gTexture;
bool gFirstFrame = true;

namespace GameLib{
	void Framework::update(){
		if ( gFirstFrame ){
			gFirstFrame = false;
			gImage = new Image( "background.dds" ); 
			createTexture( 
				&gTexture,
				gImage->width(), 
				gImage->height(), 
				gImage->data(), 
				gImage->width(), 
				gImage->height() );
			SAFE_DELETE( gImage );
		}
		setTexture( gTexture );
		double p0[ 2 ] = { 100.0, 100.0 };
		double p1[ 2 ] = { 200.0, 120.0 };
		double p2[ 2 ] = { 120.0, 200.0 };
		double t0[ 2 ] = { 0.0, 0.0 };
		double t1[ 2 ] = { 1.0, 0.0 };
		double t2[ 2 ] = { 0.0, 1.0 };
		drawTriangle2D( p0, p1, p2, t0, t1, t2, 0xffffff00, 0xff00ffff, 0xffff00ff );		
		//èIóπèàóùÇµÇƒÇ›ÇÊÇ§Ç©
		if ( isEndRequested() ){
			destroyTexture( &gTexture );
		}
	}
}
