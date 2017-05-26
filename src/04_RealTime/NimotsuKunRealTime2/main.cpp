#include "GameLib/Framework.h"
using namespace GameLib;

#include "State.h"
#include "File.h"

//関数プロトタイプ
void mainLoop();

//グローバル変数
State* gState = 0;
bool gPrevInputS = false;
bool gPrevInputA = false;
bool gPrevInputW = false;
bool gPrevInputZ = false;

//ユーザ実装関数。中身はmainLoop()に丸投げ
namespace GameLib{
	void Framework::update(){
		mainLoop();
	}
}

void mainLoop(){
	Framework f = Framework::instance();
	//最初のフレームは初期化
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
	bool inputA = f.isKeyOn( 'a' );
	bool inputS = f.isKeyOn( 's' );
	bool inputW = f.isKeyOn( 'w' );
	bool inputZ = f.isKeyOn( 'z' );
	if ( inputA && ( !gPrevInputA ) ){
		dx -= 1;
	}else if ( inputS && ( !gPrevInputS ) ){
		dx += 1;
	}else if ( inputW && ( !gPrevInputW ) ){
		dy -= 1;
	}else if ( inputZ && ( !gPrevInputZ ) ){
		dy += 1;
	}
	gPrevInputA = inputA;
	gPrevInputS = inputS;
	gPrevInputW = inputW;
	gPrevInputZ = inputZ;
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


