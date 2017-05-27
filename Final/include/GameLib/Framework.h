#ifndef INCLUDED_GAMELIB_FRAMEWORK_H
#define INCLUDED_GAMELIB_FRAMEWORK_H

#include <string>
using namespace std;

namespace GameLib{

namespace Graphics{
	class Texture;
}

namespace Math{
	class Vector3;
	class Vector2;
	class Matrix44;
}

class Framework{
public:
	enum LoadMode{
		LOAD_ARCHIVE_FIRST,
		LOAD_DIRECT_FIRST,
		LOAD_ARCHIVE_ONLY,
	};
	class Configuration{
	public:
		///幅設定
		void setWidth( int );
		///高さ設定
		void setHeight( int );
		///ウィンドウタイトル設定
		void setTitle( const char* title );
		///垂直同期有効化設定
		void enableVSync( bool = true );
		///フルスクリーンアンチエイリアス設定
		void enableAntiAlias( bool = true );
		///起動時のフルスクリーンフラグ設定
		void enableFullScreen( bool = true );
		///フルスクリーン化を禁止する
		void forbidFullScreen( bool = true );
		///ドラッグアンドドロップ有効化
		void enableDragAndDrop( bool = true );
		///アーカイブ数決定
		void setArchiveNumber( int );
		///アーカイブ名設定
		void setArchiveName( int index, const char* );
		///ロードモード設定
		void setLoadMode( LoadMode );
		///何かファイルをロード
		void loadFile( string* out, const char* filename ) const;
		///コマンドラインオプションを取得
		const char* commandLineString() const;
		///HALT,ASSERTで即死せずに例外を投げる機能の有効化。売り物に即死コードは入れられない、という人のためのもの。
		void enableThrowWhenHalt( bool = true );
		///追加スレッド数設定
		void setExtraThreadNumber( int );
	};
	Framework();
	///インスタンス取得
	static Framework instance();

	///ユーザ定義関数。作らないとリンクエラー
	void configure( Configuration* );
	///ユーザ定義関数。この中でなにもかもやる
	void update(); 

	///フルスクリーンフラグ設定。
	void enableFullScreen( bool );
	///画面幅取得
	int width() const;
	///画面高さ取得
	int height() const;
	///アプリ名取得
	const char* title() const;
	///フルスクリーンですか？
	bool isFullScreen() const;
	///フルスクリーン化は禁止されてますか？
	bool isFullScreenForbidden() const;
	///ドラッグアンドドロップは有効ですか？
	bool isDragAndDropEnabled() const;
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

	//以下ライブラリ使用者は知らなくていい
	void start( void* windowHandle );
	void preUpdate();
	void postUpdate();
	static void create();
	static void destroy();
};

} //namespace GameLib

//ベースサービス有効化
#include "GameLib/GameLib.h"

#endif
