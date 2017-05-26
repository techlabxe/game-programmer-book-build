#include "Image.h"
#include "File.h"
#include "Vector2.h"

#include "GameLib/Framework.h"
using namespace GameLib;

static int powerOfTwo( int a ){
	ASSERT( a < 0x40000000 ); //aが0x40000000より大きいと、無限ループになる。試してみよう。これが答え。
	int r = 1;
	while ( r < a ){
		r *= 2;
	}
	return r;
}

Image::Image( const char* filename ) : 
mWidth( 0 ),
mHeight( 0 ),
mTextureWidth( 0 ),
mTextureHeight( 0 ),
mTexture( 0 ){
	File f( filename );
	mHeight = f.getUnsigned( 12 );
	mWidth = f.getUnsigned( 16 );
	//一時バッファにコピー。char*から直接createTextureは危なすぎる。
	unsigned* buffer = new unsigned[ mWidth * mHeight ];
	for ( int i = 0; i < mWidth * mHeight; ++i ){
		buffer[ i ] = f.getUnsigned( 128 + i * 4 );
	}
	//createTextureします。
	//そのために、2の冪乗(べきじょう)した解像度を求める必要がある。
	mTextureWidth = powerOfTwo( mWidth );
	mTextureHeight = powerOfTwo( mHeight );

	Framework::instance().createTexture(
		&mTexture,
		mTextureWidth,
		mTextureHeight,
		buffer,
		mWidth,
		mHeight );
	SAFE_DELETE_ARRAY( buffer ); //もういらない
}

Image::~Image(){
	Framework::instance().destroyTexture( &mTexture );
}

int Image::width() const {
	return mWidth;
}

int Image::height() const {
	return mHeight;
}

//アルファブレンド付き
void Image::draw(
int dstX, 
int dstY, 
int srcX, 
int srcY, 
int width, 
int height ) const {
	//x,yの範囲を計算
	double x0 = static_cast< double >( dstX );
	double y0 = static_cast< double >( dstY );
	double x1 = x0 + static_cast< double >( width );
	double y1 = y0 + static_cast< double >( height );
	//移動後頂点
	Vector2 p0( x0, y0 );
	Vector2 p1( x1, y0 );
	Vector2 p2( x0, y1 );
	Vector2 p3( x1, y1 );
	//テクスチャ座標生成
	double rcpTw = 1.0 / static_cast< double >( mTextureWidth );
	double rcpTh = 1.0 / static_cast< double >( mTextureHeight );
	double u0 = static_cast< double >( srcX ) * rcpTw;
	double u1 = static_cast< double >( srcX + width ) * rcpTw;
	double v0 = static_cast< double >( srcY ) * rcpTh;
	double v1 = static_cast< double >( srcY + height ) * rcpTh;
	Vector2 t0( u0, v0 );
	Vector2 t1( u1, v0 );
	Vector2 t2( u0, v1 );
	Vector2 t3( u1, v1 );
	
	Framework f = Framework::instance();
	//テクスチャセット
	f.setTexture( mTexture );
	//線形合成
	f.setBlendMode( Framework::BLEND_LINEAR );
	//描画
	f.drawTriangle2D( &p0.x, &p1.x, &p2.x, &t0.x, &t1.x, &t2.x );
	f.drawTriangle2D( &p3.x, &p1.x, &p2.x, &t3.x, &t1.x, &t2.x );
}

void Image::draw() const {
	draw( 0, 0, 0, 0, mWidth, mHeight );
}

