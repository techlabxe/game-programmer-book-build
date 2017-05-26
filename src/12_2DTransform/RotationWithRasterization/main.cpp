#include <sstream>
#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Image.h"
#include "Vector2.h"
#include "Matrix22.h"
#include "GameLib/Math.h"
using namespace std;

void rotate(
Vector2* out,
const Vector2& in,
const Vector2& offset,
const Matrix22& matrix ){
	Vector2 tmpOffset( -0.5, -0.5 );
	tmpOffset += offset;
	//原点をずらして
	out->setSub( in, tmpOffset );
	//行列をかけ、
	matrix.multiply( out, *out );
	//原点を元に戻して添え字に変換
	*out += tmpOffset;
}

bool gFirstFrame = true;
Image* gImage;
int gCount;

//三角形をラスタライズ
void rasterize( 
const Vector2& a, 
const Vector2& b, 
const Vector2& c, 
const Vector2& ai,
const Vector2& bi,
const Vector2& ci,
const Image* image ){
	//abcでyが小さい順に並べる。
	Vector2 p0, p1, p2;
	Vector2 i0, i1, i2;
	if ( a.y < b.y ){
		if ( b.y < c.y ){ //abc
			p0 = a; p1 = b; p2 = c;
			i0 = ai; i1 = bi; i2 = ci;
		}else if ( a.y < c.y ){ //acb
			p0 = a; p1 = c; p2 = b;
			i0 = ai; i1 = ci; i2 = bi;
		}else{ //cab
			p0 = c; p1 = a; p2 = b;
			i0 = ci; i1 = ai; i2 = bi;
		}
	}else{
		if ( a.y < c.y ){ //bac
			p0 = b; p1 = a; p2 = c;
			i0 = bi; i1 = ai; i2 = ci;
		}else if ( b.y < c.y ){ //bca
			p0 = b; p1 = c; p2 = a;
			i0 = bi; i1 = ci; i2 = ai;
		}else{ //cba
			p0 = c; p1 = b; p2 = a;
			i0 = ci; i1 = bi; i2 = ai;
		}
	}
	//p0とp2を結ぶ辺とy=p1.yの水平線の交点を求める。
	//yは当然p1.yだから、xだけ求めればいい。
	//うまく相似を使えば
	//x = p0x + (p2x-p0x) * (p1y-p0y)/(p2y-p0y);
	//であることがわかる。p1.yがp0.yとp2.yの真ん中にあればp2x-p0xを半分にして
	//p0xに足してやればいいということだ。
	Vector2 p01, p02, p03;
	p01.setSub( p1, p0 );
	p02.setSub( p2, p0 );

	Vector2 p3;
	double t = ( p1.y - p0.y ) / p02.y; //これからtは一時的な用途にいろいろ使う
	p3.x = p0.x + p02.x * t;
	p3.y = p1.y;

	p03.setSub( p3, p0 );

	//VRAMゲット
	unsigned* vram = Framework::instance().videoMemory();
	int ww = Framework::instance().width();

	//画像側の添え字と画面上の位置の関係を式にしておく。
	//x,yがわかればix,iyがわかるようにしておくことが必要だ。
	/*
	ix,iyはxとyの一次式で表せるはずなので、

	ix = ax + by + c
	iy = dx + ey + f

	と書ける。未知数が6つだから式は6本必要だ。
	3つuvがあるのだからこれで足りるはずである。
	i0x = p0x*a + p0y*b + c
	i1x = p1x*a + p1y*b + c
	i2x = p2x*a + p2y*b + c

	i0y = p0x*d + p0y*e + f
	i1y = p1x*d + p1y*e + f
	i2y = p2x*d + p2y*e + f

	3本づつ解いてやればいい。
	i01x = p01x*a + p01y*b
	i02x = p02x*a + p02y*b

	対称性を利用してやれば
	a = ( i01x*p02y - i02x*p01y ) / ( p01x*p02y - p02x*p01y )
	b = ( i01x*p02x - i02x*p01x ) / ( p01y*p02x - p02y*p01x )
	c = uv0x - p0x*a - p0y*b
	d = ( i01y*p02y - i02y*p01y ) / ( p01x*p02y - p02x*p01y )
	e = ( i01y*p02x - i02y*p01x ) / ( p01y*p02x - p02y*p01x )
	f = uv0y - p0x*a - p0y*b
	*/
	double a0,a1,a2,a3,a4,a5; //abcdefの代わり
	Vector2 i01, i02;
	i01.setSub( i1, i0 );
	i02.setSub( i2, i0 );
	a0 = ( i01.x * p02.y - i02.x * p01.y ) / ( p01.x * p02.y - p02.x * p01.y );
	a1 = ( i01.x * p02.x - i02.x * p01.x ) / ( p01.y * p02.x - p02.y * p01.x );
	a2 = i0.x - p0.x * a0 - p0.y * a1;
	a3 = ( i01.y * p02.y - i02.y * p01.y ) / ( p01.x * p02.y - p02.x * p01.y );
	a4 = ( i01.y * p02.x - i02.y * p01.x ) / ( p01.y * p02.x - p02.y * p01.x );
	a5 = i0.y - p0.x * a3 - p0.y * a4;

	//では上半分の三角形から始めよう。p0,p1,p3だ。
	//まずYの範囲を整数にする。
	if ( p1.y > p0.y + 1.0 ){
		int yBegin = static_cast< int >( p0.y );
		int yEnd = static_cast< int >( p1.y );
		double rcpP01y = 1.0 / p01.y;
		for ( int y = yBegin; y <= yEnd; ++y ){
			//指定のyにおいてxの範囲を調べる。
			double yf = y;
			//p3-p0とp1-p0の二辺と今のyの水平線の交点を求めてやればいい。
			//計算は先ほどのと同じだ。
			t = ( y - p0.y ) * rcpP01y;
			double x0 = p0.x + p01.x * t; //p0.x + p01.x * ( y - p0.y ) / p01.y
			double x1 = p0.x + p03.x * t; //p0.x + p01.x * ( y - p0.y ) / p03.y ただしp01.y=p03.y。
			int xBegin, xEnd;
			if ( x0 > x1 ){
				xBegin = static_cast< int >( x1 ); 
				xEnd = static_cast< int >( x0 ); 
			}else{
				xBegin = static_cast< int >( x0 );
				xEnd = static_cast< int >( x1 ); 
			}
			for ( int x = xBegin; x <= xEnd; ++x ){
				double xf = x;
				//x,yからix,iyを求める
				Vector2 ip;
				ip.x = a0 * xf + a1 * yf + a2;
				ip.y = a3 * xf + a4 * yf + a5;
				//ピクセルゲット
				vram[ y * ww + x ] = image->pixel( ip );
			}
		}
	}
	//次は下半分の三角形。やることはほとんど同じ。
	if ( p2.y > p1.y + 1.0 ){
		Vector2 p21, p23;
		p21.setSub( p1, p2 );
		p23.setSub( p3, p2 );
		int yBegin = static_cast< int >( p1.y );
		int yEnd = static_cast< int >( p2.y );
		double rcpP21y = 1.0 / p21.y;
		for ( int y = yBegin; y <= yEnd; ++y ){
			//指定のyにおいてxの範囲を調べる。
			double yf = y;
			//p3-p2とp1-p2の二辺と今のyの水平線の交点を求めてやればいい。
			//計算は先ほどのと同じだ。
			t = ( y - p2.y ) * rcpP21y;
			double x0 = p2.x + p21.x * t; //p2.x + p21.x * ( y - p2.y ) / p21.y
			double x1 = p2.x + p23.x * t; //p2.x + p21.x * ( y - p2.y ) / p23.y ただしp21.y=p23.y。
			int xBegin, xEnd;
			if ( x0 > x1 ){
				xBegin = static_cast< int >( x1 ); 
				xEnd = static_cast< int >( x0 ); 
			}else{
				xBegin = static_cast< int >( x0 );
				xEnd = static_cast< int >( x1 ); 
			}
			for ( int x = xBegin; x <= xEnd; ++x ){
				double xf = x;
				//x,yからix,iyを求める
				Vector2 ip;
				ip.x = a0 * xf + a1 * yf + a2;
				ip.y = a3 * xf + a4 * yf + a5;
				//ピクセルゲット
				vram[ y * ww + x ] = image->pixel( ip );
			}
		}
	}
}


namespace GameLib{
	void Framework::update(){
		if ( gFirstFrame ){
			gImage = new Image( "background.dds" );
			gFirstFrame = false;
		}
		int ww = width(); //window width
		int wh = height(); //window height

		//毎フレームクリア
		unsigned* vram = videoMemory();
		for ( int i = 0; i < ww * wh; ++i ){
			vram[ i ] = 0;
		}

		int iw = gImage->width(); //image width
		int ih = gImage->height(); //image height
		//オフセットは画面中心。(160,120)
		Vector2 offset;
		offset.x = static_cast< double >( ww ) / 2.0;
		offset.y = static_cast< double >( wh ) / 2.0;
		
		double rotation = static_cast< double >( gCount );
		double sine = sin( rotation ); //今度は自分が回るのでマイナスは着かない
		double cosine = cos( rotation );
		Matrix22 matrix( cosine, -sine, sine, cosine ); //行列を作って
		//4点作る
		Vector2 a, b, c, d;
		rotate( &a, Vector2( ( ww - iw )/2, ( wh - ih )/2 ), offset, matrix ); //左上
		rotate( &b, Vector2( ( ww + iw )/2 - 1, ( wh - ih )/2 ), offset, matrix ); //右上
		rotate( &c, Vector2( ( ww - iw )/2, ( wh + ih )/2 - 1 ), offset, matrix ); //左下
		//4点目はa + (b-a) + (c-a)で計算できる。
		Vector2 ab, ac;
		ab.setSub( b, a );
		ac.setSub( c, a );
		d.setAdd( a, ab );
		d += ac;
		//a,b,c,dにおいて、画像側の添え字がいくつなのかを計算
		Vector2 ai( 0, 0 );
		Vector2 bi( iw - 1, 0 ); //(w-1)/w, 0)
		Vector2 ci( 0, ih - 1 ); //(0,(h-1)/h)
		Vector2 di( bi.x, ci.y );
		//ラスタライズ開始
		rasterize( a, b, c, ai, bi, ci, gImage ); //三角形abc
		rasterize( d, b, c, di, bi, ci, gImage ); //三角形dbc

		++gCount;

		ostringstream oss;
		oss << frameRate();
		drawDebugString( 0, 0, oss.str().c_str() );
	}
}