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
	//時刻を取得する(ミリ秒単位)
	unsigned time() const;
	//寝る(ミリ秒単位)
	void sleep( int milliSeconds ) const;
	//入力
	bool isKeyOn( int c ) const;


	//以下ライブラリ使用者は知らなくていい
	void start( void* windowHandle );
	void preUpdate();
	void postUpdate();
	static void create();
	static void destroy();
};

} //namespace GameLib

#include "GameLib/Base/DebugStream.h"

#endif
