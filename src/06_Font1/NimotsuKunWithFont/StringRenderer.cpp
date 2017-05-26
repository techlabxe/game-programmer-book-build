#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "StringRenderer.h"
#include "Image.h"

//static変数はcppに実体が必要
StringRenderer* StringRenderer::mInstance = 0;

StringRenderer::StringRenderer( const char* fontFileName ) : mImage( 0 ){
	mImage = new Image( fontFileName );
}

StringRenderer::~StringRenderer(){
	SAFE_DELETE( mImage );
}

void StringRenderer::create( const char* fontFileName ){
	//二度連続で呼ぶのは不正。落とす。
	STRONG_ASSERT( !mInstance && "StringRenderer::create() called twice!" );
	mInstance = new StringRenderer( fontFileName );
}

void StringRenderer::destroy(){
	SAFE_DELETE( mInstance );
}

StringRenderer* StringRenderer::instance(){
	return mInstance;
}

void StringRenderer::draw( int x, int y, const char* s, unsigned c ) const {
	//二つの定数。もしフォントによって違うならコンストラクタで受け取る必要があるが、今は固定。
	const int CHAR_WIDTH = 8;
	const int CHAR_HEIGHT = 16;
	//書き込み先を初期化
	int dstX = x * CHAR_WIDTH;
	int dstY = y * CHAR_HEIGHT;
	//Framework取得
	Framework f = Framework::instance();
	//範囲外チェック(Yが外れたら問答無用で抜ける)
	if ( dstY < 0 || dstY >= ( int )f.height() ){
		return;
	}
	//画像幅
	for ( int i = 0; s[ i ] != '\0'; ++i ){
		//X範囲判定
		if ( dstX >= 0 && ( dstX + CHAR_WIDTH ) < f.width() ){
			//文字範囲チェック
			int t = s[ i ];
			if ( t < 32 || t >= 128 ){
				t = 127; //範囲外は四角に変換
			}
			t -= 32; //フォント画像の左上が32番なので、32を引く
			int srcX = ( t % 16 ) * CHAR_WIDTH; //Xは幅で割った余り
			int srcY = ( t / 16 ) * CHAR_HEIGHT; //Yは幅で割った商
			mImage->drawWithFixedColor( dstX, dstY, srcX, srcY, CHAR_WIDTH, CHAR_HEIGHT, c );
		}
		dstX += CHAR_WIDTH;
	}
}



