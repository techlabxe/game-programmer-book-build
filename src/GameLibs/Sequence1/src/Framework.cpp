#include "GameLib/GameLib.h"
#include "GameLib/WindowCreator/WindowCreator.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/FileIO/Manager.h"
#include "GameLib/Sound/Manager.h"
#include "GameLib/Graphics/Manager.h"
#include "GameLib/Graphics/Texture.h"
#include "GameLib/Math/Vector2.h"
#include "GameLib/Scene/StringRenderer.h"
#include "GameLib/Scene/Font.h"
#include "GameLib/Threading/Functions.h"
#include "GameLib/Threading/Manager.h"

//サンプルライブラリ用
#include "GameLib/Input/Keyboard.h"


#include "src/Framework.h"
#include "src/FontTextureGenerated.h"

#include <string>
using namespace std;

namespace GameLib{

namespace { //無名

class Impl{
public:
	Impl() :
	mWidth( 320 ),
	mHeight( 240 ),
	mFullScreen( false ),
	mFullScreenForbidden( true ),
	mVSync( false ),
	mAntiAlias( false ),
	mTitle( "A GameLib Framework Application" ),
	mArchiveNames( 0 ),
	mArchiveNumber( 0 ),
	mLoadMode( FileIO::Manager::MODE_DIRECT_FIRST ),
	mPreviousFrameInterval( 0 ),
	mPreviousFrameTime( 0 ),
	mFrameRate( 0 ),
	mIdealFrameInterval( 0 ),
	mEndRequested( false ),
	mStarted( false ){
		//cout初期化
		cout.begin();
		//フレーム履歴リセット
		unsigned t = time();
		for ( int i = 0; i < TIME_HISTORY_SIZE; ++i ){
			mTimeHistory[ i ] = t;
		}

		mVideoMemoryWithPadding.setSize( mWidth * ( mHeight + 2 ) );
		//0初期化
		for ( int i = 0; i < mWidth * ( mHeight + 2 ); ++i ){
			mVideoMemoryWithPadding[ i ] = 0;
		}
		for ( int i = 0; i < mWidth; ++i ){
			mVideoMemoryWithPadding[ i ] = MAGIC_NUMBER;
			mVideoMemoryWithPadding[ mWidth * ( mHeight + 1 ) + i ] = MAGIC_NUMBER;
		}
	}
	~Impl(){
		if ( mArchiveNames ){
			SAFE_DELETE_ARRAY( mArchiveNames );
		}
		mDebugStringRenderer.release();
		mDebugFont.release();
		Graphics::Manager::destroy();
		Input::Manager::destroy();
		Sound::Manager::destroy();
		FileIO::Manager::destroy();
		Threading::Manager::destroy();
		//cout終了
		cout.end();
	}
	void start( void* windowHandle ){
		//スレッドシステム初期化
		Threading::Manager::create( 0 );
		//ファイルローダ初期化
		if ( mArchiveNumber > 0 ){
			Array< const char* > names( mArchiveNumber );
			for ( int i = 0; i < mArchiveNumber; ++i ){
				names[ i ] = mArchiveNames[ i ].c_str();
			}
			FileIO::Manager::create( &names[ 0 ], mArchiveNumber, mLoadMode );
			//アーカイブ名配列は不要なので廃棄
			SAFE_DELETE_ARRAY( mArchiveNames );
			mArchiveNumber = 0;
		}else{
			FileIO::Manager::create();
		}
		//音初期化
		Sound::Manager::create( windowHandle );
		//入力初期化
		Input::Manager::create( windowHandle );
		//描画初期化
		Graphics::Manager::create( windowHandle, mWidth * 2, mHeight * 2, mFullScreen, mVSync, mAntiAlias );
		//文字描画
		//アスキー部分の表(アスキー(126-32+1=95文字+63文字で158文字
		Array< Scene::Font::BitmapChar > chars( 158 );
		//アスキー部分充填
		int pos = 0;
		for ( int i = 0x20; i <= 0x7E; ++i ){
			Scene::Font::BitmapChar& c = chars[ pos ];
			c.mCode = static_cast< unsigned short >( i );
			c.mX = static_cast< unsigned short >( ( i % 16 ) * 8 );
			c.mY = static_cast< unsigned short >( ( i / 16 ) * 16 );
			c.mTexturePage = 0;
			c.mWidth = 8;
			c.mHeight = 16;
			++pos;
		}
		//半角カナ部
		for ( int i = 0xA1; i <= 0xDF; ++i ){
			Scene::Font::BitmapChar& c = chars[ pos ];
			c.mCode = static_cast< unsigned short >( i );
			c.mX = static_cast< unsigned short >( ( i % 16 ) * 8 );
			c.mY = static_cast< unsigned short >( ( i / 16 ) * 16 );
			c.mTexturePage = 0;
			c.mWidth = 8;
			c.mHeight = 16;
			++pos;
		}

		ASSERT( pos == 158 );
		int textureSize = sizeof( gFontTga );
		const char* fontDataArray[ 1 ];
		fontDataArray[ 0 ] = gFontTga;
		mDebugFont = Scene::Font::create( fontDataArray, &textureSize, 1, &( chars[ 0 ] ), 158 );
		mDebugStringRenderer = Scene::StringRenderer::create( 2048, 128 ); //こんなもんで足りるだろう？
		mDebugStringRenderer.setFont( mDebugFont );
//2Dレイヤー
		m2dTexture = Graphics::Texture::create( mWidth, mHeight, false );
		Graphics::Manager().setTextureFilter( Graphics::TEXTURE_FILTER_POINT );

		mStarted = true;
	}
	unsigned time() const {
		return WindowCreator().time();
	}
	void preUpdate(){
		unsigned currentTime = time();
		while ( ( currentTime - mPreviousFrameTime ) < mIdealFrameInterval ){
			Threading::sleep( 1 );
			currentTime = time();
		}
		mPreviousFrameTime = currentTime;

		//フレーム時間更新
		mPreviousFrameInterval = currentTime - mTimeHistory[ TIME_HISTORY_SIZE - 1 ];
		unsigned frameIntervalSum = currentTime - mTimeHistory[ 0 ];
		mFrameRate = TIME_HISTORY_SIZE * 1000 / frameIntervalSum;
		//履歴更新
		for ( int i = 0; i < TIME_HISTORY_SIZE - 1; ++i ){
			mTimeHistory[ i ] = mTimeHistory[ i + 1 ]; 
		}
		mTimeHistory[ TIME_HISTORY_SIZE - 1 ] = currentTime;

		Graphics::Manager().beginDraw();
		float pointerScale;
		Vector2 pointerOffset;
		Graphics::Manager().getPointerModifier( &pointerScale, &pointerOffset );
		Input::Manager().update( pointerScale, pointerOffset );
	}
	void postUpdate(){
		//----2D処理
		//破壊チェック
		for ( int i = 0; i < mWidth; ++i ){
			ASSERT( mVideoMemoryWithPadding[ i ] == MAGIC_NUMBER && "VRAM RANGE ERROR!" );
			ASSERT( mVideoMemoryWithPadding[ mWidth * ( mHeight + 1 ) + i ] == MAGIC_NUMBER && "VRAM RANGE ERROR!" );
		}
		//VRAMコピー
		unsigned* dst;
		int pitch;
		m2dTexture.lock( &dst, &pitch );
		const unsigned* src = &mVideoMemoryWithPadding[ mWidth ];
		for ( int y = 0; y < mHeight; ++y ){			
			for ( int x = 0; x < mWidth; ++x ){
				dst[ x ] = 0xff000000 | src[ x ]; //強制アルファff
			}
			src += mWidth;
			dst += pitch / 4;
		}
		src = 0; //使い終わり
		m2dTexture.unlock( &dst );
		Graphics::Manager().blendToScreen( m2dTexture );

		//文字合成
		mDebugStringRenderer.draw();
		Graphics::Manager().endDraw();
	}
	//サンプルライブラリ用
	Array< unsigned > mVideoMemoryWithPadding;
	Graphics::Texture m2dTexture;
	static const unsigned MAGIC_NUMBER = 0x12345678;


	int mWidth;
	int mHeight;
	bool mFullScreen;
	bool mFullScreenForbidden;
	bool mVSync;
	bool mAntiAlias;
	string mTitle;
	string* mArchiveNames;
	int mArchiveNumber;
	FileIO::Manager::AccessMode mLoadMode;
	static const int TIME_HISTORY_SIZE = 60;
	unsigned mTimeHistory[ TIME_HISTORY_SIZE ];
	int mPreviousFrameInterval;
	unsigned mPreviousFrameTime;
	int mFrameRate;
	unsigned mIdealFrameInterval;
	bool mEndRequested;
	bool mStarted;
	Scene::StringRenderer mDebugStringRenderer;
	Scene::Font mDebugFont;
};

Impl* gImpl = 0;

} //namespace{}

Framework::Framework(){
	//別スレッドからの呼び出しは許さない
	ASSERT( WindowCreator::isMainThread() && "you must call from MAIN thread" );
}

void Framework::create(){
	ASSERT( !gImpl );
	gImpl = NEW Impl();
}

void Framework::destroy(){
	SAFE_DELETE( gImpl );
}

Framework Framework::instance(){
	return Framework();
}

void Framework::start( void* windowHandle ){
	gImpl->start( windowHandle );
}

void Framework::preUpdate(){
	gImpl->preUpdate();
}

void Framework::postUpdate(){
	gImpl->postUpdate();
}

int Framework::width() const {
	return gImpl->mWidth;
}

int Framework::height() const {
	return gImpl->mHeight;
}
/*
const char* Framework::getTitle() const {
	return gImpl->mTitle.c_str();
}

bool Framework::isFullScreen() const {
	return gImpl->mFullScreen;
}

bool Framework::isFullScreenForbidden() const {
	return gImpl->mFullScreenForbidden;
}
*/
void Framework::requestEnd(){
	gImpl->mEndRequested = true;
}

bool Framework::isEndRequested() const {
	return gImpl->mEndRequested;
}

unsigned Framework::time() const {
	return gImpl->time();
}

int Framework::previousFrameInterval() const {
	return gImpl->mPreviousFrameInterval;
}

int Framework::frameRate() const {
	return gImpl->mFrameRate;
}

void Framework::setFrameRate( int fr ){
	gImpl->mIdealFrameInterval = 1000 / fr;
}

/*
void Framework::drawDebugString( int c, int r, const char* s, unsigned col ){
	gImpl->mDebugStringRenderer.add( c * 8, r * 16, s, col );
}

void Framework::enableFullScreen( bool f ){ 
	if ( gImpl->mFullScreenForbidden ){ //禁止されている
		return;
	}
	WindowCreator wc = WindowCreator::getInstance();
	Graphics::Manager gm = Graphics::Manager::getInstance();
	if ( gImpl->mStarted ){
		//中途変更
		if ( gImpl->mFullScreen != f ){
			//上流へ
			wc.enableFullScreen( f );
			//下流へ
			gm.enableFullScreen( f );
		}
	}
	gImpl->mFullScreen = f;
}

//Configuration
void Framework::Configuration::setWidth( int width ){
	gImpl->mWidth = width;
}

void Framework::Configuration::setHeight( int height ){
	gImpl->mHeight = height;
}

void Framework::Configuration::setTitle( const char* title ){
	gImpl->mTitle = title;
}

void Framework::Configuration::setArchiveNumber( int n ){
	ASSERT( !gImpl->mArchiveNames && "you can't call twice." );
	gImpl->mArchiveNumber = n;
	gImpl->mArchiveNames = NEW string[ n ];
}

void Framework::Configuration::setArchiveName( int index, const char* name ){
	ASSERT( gImpl->mArchiveNames && "you must call setArchiveNumber() before this call." );
	ASSERT( index >= 0 && index < gImpl->mArchiveNumber );
	gImpl->mArchiveNames[ index ] = name;
}

void Framework::Configuration::setLoadMode( LoadMode lm ){
	using namespace FileIO;
	Manager::AccessMode am = Manager::MODE_DIRECT_FIRST;
	switch ( lm ){
		case LOAD_ARCHIVE_FIRST: am = Manager::MODE_ARCHIVE_FIRST; break;
		case LOAD_DIRECT_FIRST: am = Manager::MODE_DIRECT_FIRST; break;
		case LOAD_ARCHIVE_ONLY: am = Manager::MODE_ARCHIVE_ONLY; break;
		default: ASSERT( false ); break;
	}
	gImpl->mLoadMode = am;
}

void Framework::Configuration::enableFullScreen( bool f ){
	gImpl->mFullScreen = f;
}

void Framework::Configuration::enableVSync( bool f ){
	gImpl->mVSync = f;
}

void Framework::Configuration::enableAntiAlias( bool f ){
	gImpl->mAntiAlias = f;
}

void Framework::Configuration::forbidFullScreen( bool f ){
	gImpl->mFullScreenForbidden = f;
}
*/

//サンプルライブラリ用関数
unsigned* Framework::videoMemory(){
	return &gImpl->mVideoMemoryWithPadding[ gImpl->mWidth ];
}

bool Framework::isKeyOn( int c ) const {
	return Input::Manager().keyboard().isOn( c );
}

bool Framework::isKeyTriggered( int c ) const {
	return Input::Manager().keyboard().isTriggered( c );
}

void Framework::sleep( int ms ) const {
	Threading::sleep( ms );
}



//WindowProcedureのユーザ実装関数
void WindowCreator::configure( Configuration* config ){
	Framework::create();
	//ユーザ設定
	Framework f;
//	Framework::Configuration fwConfig;
//	f.configure( &fwConfig );
	config->setWidth( f.width() * 2 );
	config->setHeight( f.height() * 2 );
	config->setTitle( "Sequence1 samples" );
	config->enableFullScreen( false );
	config->forbidFullScreen( true );
}

void WindowCreator::update(){
	WindowCreator wc;
	Graphics::Manager gm;
	Framework f;
	//WindowCreatorの状態次第でいろいろする
	bool sleepFlag = false;
	//最小化していれば寝る
	if ( wc.isMinimized() ){
		sleepFlag = true;
	}
	//デバイスロストこいているようなら復帰を試みるが、基本的に寝る。
/*
	if ( !gm.canRender() ){
		gm.restore();
		sleepFlag = true;
	}
	//Windowからモード切替シグナル
	bool wcFullScreen = wc.isFullScreen();
	if ( f.isFullScreen() != wcFullScreen ){
		f.enableFullScreen( wcFullScreen );
	}
*/
	if ( !sleepFlag ){
		f.preUpdate();
	}
	//上流(×ボタンとか)から終了命令が来ているかチェック
	if ( isEndRequested() ){ //来ていればFrameworkに終了命令を伝達
		f.requestEnd();
	}
	if ( sleepFlag ){
		Threading::sleep( 50 ); //50ms寝る
	}else{
		f.update();
		f.postUpdate();
	}
	//終了判定
	if ( f.isEndRequested() ){
		Framework::destroy();
		end(); //上流にも知らせを
	}
}

void WindowCreator::start( void* windowHandle ){
	Framework().start( windowHandle );
}

/*
//StandardInput
StandardInput cin;

StandardInput::StandardInput() : mPrevEnterOn( false ){
}

StandardInput& StandardInput::operator>>( char& a ){
	a = '\0'; //初期値
	Input::Keyboard kb = Input::Manager::getInstance().getKeyboard( 0 );
	bool entered = false;
	while ( true ){ //無限ループ
		for ( int i = 0; i < 0x80; ++i ){ //キー全部見る
			if ( kb.isOn( i ) ){
				a = static_cast< char >( i );
				entered = true;
			}
		}
		bool enterOn = kb.isOn( Input::Keyboard::KEY_RETURN );
		if ( entered && enterOn && !mPrevEnterOn ){
			mPrevEnterOn = enterOn;
			break;
		}
		float pointerScale;
		Vector2 pointerOffset;
		Graphics::Manager::getInstance().getPointerModifier( &pointerScale, &pointerOffset );
		Input::Manager::getInstance().update( pointerScale, pointerOffset );
		//ウィンドウから終了要請を受けているようなら抜ける
		bool altF4 = kb.isOn( Input::Keyboard::KEY_ALT ) && kb.isOn( Input::Keyboard::KEY_F4 );
		if ( altF4 ){
			Framework::getInstance().requestEnd();
			break;
		}
		mPrevEnterOn = enterOn;
	}
	return *this;
}
*/

} //namespace GameLib
