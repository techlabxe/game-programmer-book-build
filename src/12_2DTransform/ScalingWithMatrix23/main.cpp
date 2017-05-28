#include <sstream>
#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "Image.h"
#include "Vector2.h"
#include "Matrix23.h"
#include "GameLib/GameLibMath.h"
using namespace std;

int round( double a ){
	a += ( a > 0.0 ) ? 0.5 : -0.5f;
	return static_cast< int >( a );
}

bool gFirstFrame = true;
Image* gImage;
int gCount;

namespace GameLib{
	void Framework::update(){
		if ( gFirstFrame ){
			gImage = new Image( "background.dds" );
			gFirstFrame = false;
		}
		unsigned* vram = videoMemory();
		int ww = width(); //window width
		int wh = height(); //window height
		//ˆê’U‘S•”^‚Á•‚É
		for ( int i = 0; i < ww * wh; ++i ){
			vram[ i ] = 0;
		}
		int iw = gImage->width(); //image width
		int ih = gImage->height(); //image height

		//Šg‘åk¬s—ñ‚ğì‚Á‚Ä‚İ‚éB
		double sx = static_cast< double >( gCount % 199 ) / 100.0;
		double sy = static_cast< double >( gCount % 113 ) / 60.0;
		Matrix23 matrix( 
			sx, 0.0, 0.0,
			0.0, sy, 0.0 );
		//3“_ì‚é
		Vector2 a, b, c;
		matrix.multiply( &a, Vector2( 0, 0 ) );
		matrix.multiply( &b, Vector2( iw, 0 ) );
		matrix.multiply( &c, Vector2( 0, ih ) );
		//b-a,c-a‚ğŒvZ
		Vector2 ab, ac;
		ab.setSub( b, a );
		ac.setSub( c, a );
		//•âŠÔŠJn
		double rcpWidth = 1.0 / static_cast< double >( iw );
		double rcpHeight = 1.0 / static_cast< double >( ih );
		for ( int y = 0; y < ih; ++y ){
			double yf = static_cast< double >( y ) + 0.5;
			double v = yf * rcpHeight;
			for ( int x = 0; x < iw; ++x ){
				double xf = static_cast< double >( x ) + 0.5;
				double u = xf * rcpWidth;
				Vector2 p;
				p.setInterporation( a, ab, ac, u, v );
				p -= Vector2( 0.5, 0.5 ); //“Y‚¦š‚Ö
				int rx, ry;
				rx = round( p.x );
				ry = round( p.y );
				//”ÍˆÍ“à‚È‚ç“\‚è•t‚¯
				if ( rx >= 0 && rx < ww && ry >= 0 && ry < wh ){
					vram[ ry * ww + rx ] = gImage->pixel( x, y );
				}
			}
		}
		++gCount;

		ostringstream oss;
		oss << frameRate();
		drawDebugString( 0, 0, oss.str().c_str() );
	}
}
