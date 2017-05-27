#ifndef INCLUDED_GAMELIB_WINDOWCREATOR_H
#define INCLUDED_GAMELIB_WINDOWCREATOR_H

namespace GameLib{

class WindowCreator{
public:
	class Configuration{
	public:
		void setWidth( int width );
		void setHeight( int height );
		void setTitle( const char* title );
		void forbidFullScreen( bool = true ); //これを呼んでおくとalt-enterが無視される
		void enableFullScreen( bool = true );
		void enableDragAndDrop( bool = true );
	};
	WindowCreator();
	//ユーザ実装関数
	void configure( Configuration* );
	void start( void* windowHandle );
	void update();

	const char* title() const;
	int titleLength() const;
	int width() const;
	int height() const;
	bool isFullScreen() const;
	bool isMinimized() const;
	bool isActive() const;
	void enableFullScreen( bool );
	///コマンドライン文字列取得
	const char* commandLineString() const;
	///ドラッグアンドドロップは有効ですか？
	bool isDragAndDropEnabled() const;
	///ドラッグアンドドロップ数取得
	int droppedItemNumber() const;
	///ドラッグアンドドロップ取得
	const char* droppedItem( int index ) const;
	///ドラッグアンドドロップクリア
	void clearDroppedItem();
	///ミリ秒単位時刻を返す
	unsigned time() const;

	static bool isMainThread();
	static WindowCreator instance();
	void requestEnd(); //終わりを要求する
	void end(); //終わりを通知する。
	bool isEndRequested() const; //終わりは要求されてますか
	int getAndResetMouseWheel();
};

} //namespace GameLib

#endif