#ifndef INCLUDED_GAMELIB_FRAMEWORK_H
#define INCLUDED_GAMELIB_FRAMEWORK_H

namespace GameLib{

class Texture;

class Framework{
public:
	Framework();
	///ユーザ定義関数。これを書く。
	void update();
	///インスタンス取得
	static Framework instance();
	///画面幅取得
	int width() const;
	///画面高さ取得
	int height() const;
	///Frameworkに終了命令を出す
	void requestEnd();
	///Frameworkに終了命令が出ているか調べる
	bool isEndRequested() const;
	///時刻を取得する(ミリ秒単位)
	unsigned time() const;
	///フレームレート取得
	int frameRate() const;
	///フレームレート設定(内部で待ちループを回してくれる)
	void setFrameRate( int );
	///直前フレーム時間取得(ミリ秒単位)
	int previousFrameInterval() const;
	///ランダム数ゲット
	int getRandom( int max = 0 );
	//文字を書く
	void drawDebugString( 
		int column, //列(x)
		int row, //行(y)
		const char* string, 
		unsigned color = 0xffffffff );
	///三角形を描く。同次座標で与えるので、pは4要素。tは2要素。
	void drawTriangle3DH(
		const double* p0,
		const double* p1,
		const double* p2,
		const double* t0 = 0,
		const double* t1 = 0,
		const double* t2 = 0,
		unsigned c0 = 0xffffffff,
		unsigned c1 = 0xffffffff,
		unsigned c2 = 0xffffffff );
	//テクスチャを作る
	void createTexture(
		Texture** textureOut,
		const char* filename );
	//テクスチャを破棄する
	void destroyTexture( Texture** );
	//テクスチャをセットする
	void setTexture( const Texture* );
	//テクスチャのサイズを調べる
	void getTextureSizes( const Texture*, int* width, int* height, int* originalWidth, int* originalHeight );
	//ブレンドモード
	enum BlendMode{ 
		BLEND_LINEAR,
		BLEND_ADDITIVE,
		BLEND_OPAQUE,
	};
	void setBlendMode( BlendMode );
	void enableDepthTest( bool );
	void enableDepthWrite( bool );

	//以下ライブラリ使用者は知らなくていい
	void start( void* windowHandle );
	void preUpdate();
	void postUpdate();
	static void create();
	static void destroy();
};

} //namespace GameLib

#include "GameLib/GameLib.h"

#endif
