#include <sstream>
#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "Image.h"
#include "GameLib/Math.h"
using namespace GameLib;
using namespace std;

int round( double a ){
	a += ( a > 0.0 ) ? 0.5 : -0.5f;
	return static_cast< int >( a );
}

void rotate(
int* rx,
int* ry,
int x,
int y,
double xOffset,
double yOffset,
double sine,
double cosine ){
	double xf = static_cast< double >( x );
	double yf = static_cast< double >( y );
	//オフセットと0.5をまとめる
	double tmpOffsetX = xOffset - 0.5;
	double tmpOffsetY = yOffset - 0.5;
	//添え字から座標に変換して、原点をずらす
	xf -= tmpOffsetX;
	yf -= tmpOffsetY;
	//式をまんま当てはめる
	double xf2 = cosine * xf - sine * yf;
	double yf2 = sine * xf + cosine * yf;
	xf = xf2;
	yf = yf2;
	//原点を元に戻して添え字へ
	xf += tmpOffsetX;
	yf += tmpOffsetY;
	//四捨五入して整数化
	*rx = round( xf );
	*ry = round( yf );
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
		//一旦全部真っ黒に
		for ( int i = 0; i < ww * wh; ++i ){
			vram[ i ] = 0;
		}
		int iw = gImage->width(); //image width
		int ih = gImage->height(); //image height
		double offsetX = static_cast< double >( iw ) / 2.0;
		double offsetY = static_cast< double >( ih ) / 2.0;
		double rotation = static_cast< double >( gCount );
		double sine = sin( rotation );
		double cosine = cos( rotation );
		for ( int y = 0; y < ih; ++y ){
			for ( int x = 0; x < iw; ++x ){
				//回転先を計算
				int rx, ry;
				rotate( &rx, &ry, x, y, offsetX, offsetY, sine, cosine );
				//範囲内なら貼り付け
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

