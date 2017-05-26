#ifndef INCLUDED_GAMELIB_FRAMEWORK_H
#define INCLUDED_GAMELIB_FRAMEWORK_H

namespace GameLib{

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
	///VRAM取得
	unsigned* videoMemory();
	///Frameworkに終了命令を出す
	void requestEnd();
	///Frameworkに終了命令が出ているか調べる
	bool isEndRequested() const;
	///ミリ秒単位で寝る
	void sleep( int milliSeconds ) const;
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
