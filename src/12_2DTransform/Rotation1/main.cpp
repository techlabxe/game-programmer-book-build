#include <sstream>
#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "Image.h"
#include "GameLib/Math.h"
#include <sstream>
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
double rotation ){
	double xf = static_cast< double >( x );
	double yf = static_cast< double >( y );
	//添え字から座標に変換
	xf += 0.5f;
	yf += 0.5f;
	//原点をずらす
	xf -= xOffset;
	yf -= yOffset;
	//角度と半径を求める
	double r = GameLib::sqrt( xf * xf + yf * yf );
	double angle = GameLib::atan2( yf, xf );
	//角度にrotationを加える。
	angle += rotation;
	//サインコサイン
	double sine = GameLib::sin( angle );
	double cosine = GameLib::cos( angle );
	//xf,yfを計算しなおし
	xf = r * cosine;
	yf = r * sine;
	//原点を元に戻す
	xf += xOffset;
	yf += yOffset;
	//座標から添え字へ
	xf -= 0.5;
	yf -= 0.5;
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
		for ( int y = 0; y < ih; ++y ){
			for ( int x = 0; x < iw; ++x ){
				//回転先を計算
				int rx, ry;
				rotate( &rx, &ry, x, y, offsetX, offsetY, rotation );
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