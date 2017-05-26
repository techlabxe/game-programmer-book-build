#include "GameLib/Framework.h"
using namespace GameLib;

#include "State.h"
#include "File.h"

//関数プロトタイプ
void mainLoop();

//グローバル変数
State* gState = 0;
const int gFrameInterval = 16; //16ミリ秒。62.5fps。
unsigned gPreviousTime[ 10 ]; //10フレーム分の時刻を記録
int gCounter = 0; //メインループを回った回数を数えるカウンタ

#define USE_SLEEP_FUNCTION //sleep関数使用

//ユーザ実装関数。中身はmainLoop()に丸投げ
namespace GameLib{
	void Framework::update(){
		mainLoop();
	}
}

void mainLoop(){
	//フレームレート一定化処理。取得時刻巻き戻り対策付き
	Framework f = Framework::instance();
	while ( ( f.time() - gPreviousTime[ 9 ] ) < gFrameInterval ){
#ifdef USE_SLEEP_FUNCTION
		f.sleep( 1 );
#endif
	}
	//フレームレート計算
	unsigned currentTime = f.time();
	unsigned frameInterval10 = currentTime - gPreviousTime[ 0 ];
	if ( gCounter % 60 == 0 ){ //60フレームに一回フレームレート表示
		cout << "frameInterval:" << frameInterval10/10;
		cout << " FrameRate:" << 10*1000/frameInterval10 << endl;
	}
	++gCounter;
	//履歴更新
	for ( int i = 0; i < 9; ++i ){
		gPreviousTime[ i ] = gPreviousTime[ i + 1 ];
	}
	gPreviousTime[ 9 ] = currentTime;

	//最初のフレームは初期化。
	if ( !gState ){ 
		File file( "stageData.txt" );
		if ( !( file.getData() ) ){ //データない！
			cout << "stage file could not be read." << endl;
			return;
		}
		gState = new State( file.getData(), file.getSize() );
	}
	bool cleared = false;
	//メインループ
	//クリアチェック
	if ( gState->hasCleared() ){
		cleared = true;
	}
	//入力取得
	int dx = 0;
	int dy = 0;
	if ( f.isKeyOn( 'a' ) ){
		dx -= 1;
	}else if ( f.isKeyOn( 's' ) ){
		dx += 1;
	}else if ( f.isKeyOn( 'w' ) ){
		dy -= 1;
	}else if ( f.isKeyOn( 'z' ) ){
		dy += 1;
	}
	//更新
	gState->update( dx, dy );
	//描画
	gState->draw();

	if ( cleared ){
		//祝いのメッセージ
		cout << "Congratulation! you win." << endl;
		delete gState;
		gState = 0;
	}
	//終了判定
	if ( f.isKeyOn( 'q' ) ){
		f.requestEnd();
	}
	if ( f.isEndRequested() ){
		if ( gState ){
			delete gState;
			gState = 0;
		}
	}
}


