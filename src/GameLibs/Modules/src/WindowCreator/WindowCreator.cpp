#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x400 //WM_MOUSEWHEELのために
#endif

#include <tchar.h>
#include <windows.h>
#include <mmsystem.h>
#include "WindowCreator/resource.h"
#include <string>
#undef min
#undef max

#include "GameLib/GameLib.h"
#include "GameLib/WindowCreator/WindowCreator.h"
#include "GameLib/Threading/Functions.h"

using namespace std;
using namespace GameLib;

//WindowCreatorメンバ関数
namespace GameLib{

namespace{ //無名

class Impl{
public:
	Impl() :
	mWidth( 640 ), 
	mHeight( 480 ), 
	mFullScreen( false ),
	mMinimized( false ),
	mActive( false ),
	mTitle( "A GameLib Application" ),
	mCommandLine( "" ),
	mEndRequested( false ),
	mEnded( false ),
	mStarted( false ),
	mFullScreenForbidden( false ),
	mDragAndDropEnabled( false ),
	mMouseWheel( 0 ),
	mTimeBias( 0 ){
		//getTime()用バイアス値計算
		unsigned time = timeGetTime();
		mTimeBias = 0xffffffff - time;
		mTimeBias -= 60000; //60秒引く。

		//スレッドID取得
		mThreadId = GetCurrentThreadId();
	}
	~Impl(){
	}
	void enableFullScreen( bool f ){
		if ( mFullScreen != f ){
			if ( mStarted ){
				LONG style;
				if ( f ){
					style = WS_POPUP | WS_VISIBLE;
					//最大化するときには今の場所を覚えておく
					GetWindowRect( mWindowHandle, &mWindowRect );
					SetWindowLong( mWindowHandle, GWL_STYLE, style );
					//位置は0,0
					SetWindowPos( mWindowHandle, NULL, 
						0, 0,
						mWidth, mHeight,
						SWP_SHOWWINDOW );
				}else{
					style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;
					SetWindowLong( mWindowHandle, GWL_STYLE, style );
					//元に戻す時には位置を戻す
					SetWindowPos( mWindowHandle, NULL, 
						mWindowRect.left, mWindowRect.top,
						mWindowRect.right - mWindowRect.left,
						mWindowRect.bottom - mWindowRect.top,
						SWP_SHOWWINDOW );
				}
			}
			mFullScreen = f;
		}
	}
	void createDefaultWindowPos(){
		mWindowRect.left = 0;
		mWindowRect.top = 0;
		mWindowRect.right = mWidth;
		mWindowRect.bottom = mHeight;
		LONG style;
		style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;
		int oldLeft = mWindowRect.left;
		int oldTop = mWindowRect.top;
		AdjustWindowRect( &mWindowRect, style, FALSE );
		int diffX = mWindowRect.left - oldLeft;
		int diffY = mWindowRect.top - oldTop;
		mWindowRect.left -= diffX;
		mWindowRect.right -= diffX;
		mWindowRect.top -= diffY;
		mWindowRect.bottom -= diffY;
	}
	int mWidth;
	int mHeight;
	bool mFullScreen;
	bool mMinimized;
	bool mActive;
	string mTitle;
	string mCommandLine;
	bool mEndRequested;
	bool mEnded;
	bool mStarted;
	bool mFullScreenForbidden;
	bool mDragAndDropEnabled;
	int mMouseWheel;
	unsigned mTimeBias; //timeGetTimeにはかせるゲタ
	unsigned mThreadId;
	Array< string > mDroppedItems;
	HWND mWindowHandle;
	RECT mWindowRect;
};

Impl* gImpl = 0; //唯一インスタンス

} //namespace{}

WindowCreator::WindowCreator(){
	//別スレッドからの呼び出しは許さない
	ASSERT( isMainThread() && "you must call from MAIN thread" );
}

const char* WindowCreator::title() const {
	return gImpl->mTitle.c_str();
}

const char* WindowCreator::commandLineString() const {
	return gImpl->mCommandLine.c_str();
}

int WindowCreator::titleLength() const {
	return static_cast< int >( gImpl->mTitle.size() );
}

int WindowCreator::height() const {
	return gImpl->mHeight;
}

int WindowCreator::width() const {
	return gImpl->mWidth;
}

bool WindowCreator::isFullScreen() const {
	return gImpl->mFullScreen;
}

WindowCreator WindowCreator::instance(){
	return WindowCreator(); //実はこの関数いらない
}

bool WindowCreator::isEndRequested() const {
	return gImpl->mEndRequested;
}

void WindowCreator::requestEnd(){
	gImpl->mEndRequested = true;
}

void WindowCreator::end(){
	gImpl->mEnded = true;
}

unsigned WindowCreator::time() const {
	return timeGetTime() + gImpl->mTimeBias;
}

bool WindowCreator::isMinimized() const {
	return gImpl->mMinimized;
}

bool WindowCreator::isActive() const {
	return gImpl->mActive;
}

bool WindowCreator::isMainThread(){
	return ( GetCurrentThreadId() == gImpl->mThreadId );
}

void WindowCreator::enableFullScreen( bool f ){
	gImpl->enableFullScreen( f );
}

bool WindowCreator::isDragAndDropEnabled() const {
	return gImpl->mDragAndDropEnabled;
}

int WindowCreator::droppedItemNumber() const {
	return gImpl->mDroppedItems.size();
}

const char* WindowCreator::droppedItem( int i ) const {
	return gImpl->mDroppedItems[ i ].c_str();
}

void WindowCreator::clearDroppedItem(){
	gImpl->mDroppedItems.clear();
}

int WindowCreator::getAndResetMouseWheel(){
	int t = gImpl->mMouseWheel;
	gImpl->mMouseWheel = 0;
	return t;
}

//Configuration
void WindowCreator::Configuration::setWidth( int w ){
	gImpl->mWidth = w;
}

void WindowCreator::Configuration::setHeight( int h ){
	gImpl->mHeight = h;
}

void WindowCreator::Configuration::setTitle( const char* title ){
	gImpl->mTitle = title;
}

void WindowCreator::Configuration::enableFullScreen( bool f ){
	gImpl->enableFullScreen( f );
}

void WindowCreator::Configuration::forbidFullScreen( bool f ){
	gImpl->mFullScreenForbidden = f;
}

void WindowCreator::Configuration::enableDragAndDrop( bool f ){
	gImpl->mDragAndDropEnabled = f;
}

} //namespace GameLib

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ここにコードを挿入してください。
	MSG msg;
	HACCEL hAccelTable;

	//ウィンドウ状態保持クラス生成
	gImpl = NEW Impl();
	gImpl->mCommandLine = lpCmdLine;
	WindowCreator wc;
	//ユーザ設定関数呼び出し
	WindowCreator::Configuration config;
	wc.configure( &config );

	memcpy(
		szTitle, 
		wc.title(), 
		wc.titleLength() ); 
	memcpy( szWindowClass, "GameLibWindowCreator", sizeof("GameLibWindowCreator"));

	MyRegisterClass( hInstance );

	// アプリケーションの初期化を実行します:
	if ( !InitInstance ( hInstance, nCmdShow ) ){
		return FALSE;
	}

	hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_WINDOWCREATOR ) );

	timeBeginPeriod( 1 );
	HWND windowHandle = gImpl->mWindowHandle;
	///起動前の例外は継続不可能なので、メッセージボックスを出して終わらせる
	try{
		wc.start( windowHandle );
	}
	catch ( ... ){ //起動時の例外は無視できない。
		MessageBoxA( 
			windowHandle, 
			"致命的な問題があり、起動できません。申し訳ありませんが終了いたします", 
			"致命的エラー", 
			MB_OK | MB_ICONERROR );
		wc.requestEnd(); //終われ。
		wc.update(); //中で終了処理
		SAFE_DELETE( gImpl );
		timeEndPeriod( 1 );
		return 0;
	}

	//フルスクリーンなら戻すウィンドウ位置を生成
	if ( gImpl->mFullScreen ){
		gImpl->createDefaultWindowPos();
	}
	//ドラッグアンドドロップ有効化
	if ( gImpl->mDragAndDropEnabled ){
		DragAcceptFiles( windowHandle, TRUE );
	}
	gImpl->mStarted = true; //ループ開始信号
	while ( true ){
		//メッセージがあるなら処理
		if ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ){
			if ( GetMessage( &msg, NULL, 0, 0 ) ){
				if ( !TranslateAccelerator( msg.hwnd, hAccelTable, &msg ) ){
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
			}else{
				break; //WM_QUITきました。ループ抜けます。
			}
		}
		//終わってればwindow破棄
		if ( gImpl->mEnded ){ //終わってなければ更新ループ
			if ( windowHandle ){ //もしかすると2回来るかもしれないので念のため
				DestroyWindow( windowHandle );
				windowHandle = 0;
			}
		}else{	//実行本体
			try{
				wc.update();
			}
			catch ( Exception e ){
				if ( e == EXCEPTION_EXIT ){ //終了要請例外の場合のみ終了
					MessageBoxA( 
						windowHandle, 
						"致命的な問題があり、継続できません。申し訳ありませんが終了いたします", 
						"致命的エラー", 
						MB_OK | MB_ICONERROR );
				}
			}
		}
	}
	SAFE_DELETE( gImpl );
	timeEndPeriod( 1 );

	CoUninitialize();
	return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWCREATOR));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WINDOWCREATOR);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // グローバル変数にインスタンス処理を格納します。
//----
	DWORD style = 0;
	if ( gImpl->mFullScreen ){
		style = WS_POPUP;
	}else{
		style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	}
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = gImpl->mWidth;
	rect.bottom = gImpl->mHeight;
	AdjustWindowRect(&rect, style, FALSE);
	rect.right -= rect.left;
	rect.bottom -= rect.top; //差分計算
	rect.left = 0;
	rect.top = 0;
//----

	hWnd = CreateWindow(szWindowClass, szTitle, 
		style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right, rect.bottom, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   gImpl->mWindowHandle = hWnd;

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		break;
	case WM_CLOSE: //終了要請
		gImpl->mEndRequested = true;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SYSKEYDOWN: //alt+enter認識
		if ( !gImpl->mFullScreenForbidden ){
			if ( wParam == VK_RETURN ){
				if ( gImpl->mFullScreen ){
					WindowCreator().enableFullScreen( false );
				}else{
					WindowCreator().enableFullScreen( true );
				}
			}
		}
		DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_ACTIVATE:
		if ( wParam & 0xffff0000 ){ //最小化されていればfalse
			gImpl->mActive = false;
		}else{
			gImpl->mActive = ( ( wParam & 0xffff ) != 0 );
		}
		break;
	case WM_SYSCOMMAND:
		if ( wParam == SC_MAXIMIZE ){
			if ( !gImpl->mFullScreenForbidden ){
				WindowCreator().enableFullScreen( true ); //最大化の時はDefWindowProcは呼ばない
			}
		}else if ( wParam == SC_MINIMIZE ){
			gImpl->mMinimized = true;
			gImpl->mActive = false;
			DefWindowProc(hWnd, message, wParam, lParam);
		}else if ( wParam == SC_RESTORE ){
			gImpl->mMinimized = false;
			gImpl->mActive = true;
			DefWindowProc(hWnd, message, wParam, lParam);
		}else{
			DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_MOUSEWHEEL:
		gImpl->mMouseWheel = ( static_cast< short >( HIWORD( wParam ) ) > 0 ) ? 1 : -1;
		break;
	case WM_DROPFILES:
		if ( gImpl->mDragAndDropEnabled ){
			HDROP hDrop = ( HDROP )wParam;
			gImpl->mDroppedItems.clear();
			unsigned n = DragQueryFileA( hDrop, 0xffffffff, NULL, 0 );
			gImpl->mDroppedItems.setSize( n );
			char item[ MAX_PATH ];
			for ( unsigned i = 0; i < n; ++i ){
				DragQueryFileA( hDrop, i, item, MAX_PATH );
				gImpl->mDroppedItems[ i ] = item;
			}
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
