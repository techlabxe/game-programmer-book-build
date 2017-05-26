#include "GameLib/Framework.h"
using namespace GameLib;

#include "State.h"
#include "File.h"

//関数プロトタイプ
void mainLoop();

//グローバル変数
State* gState = 0;

//ユーザ実装関数。中身はmainLoop()に丸投げ
namespace GameLib{
	void Framework::update(){
		mainLoop();
	}
}

void mainLoop(){
	//×ボタン押されてる？
	if ( Framework::getInstance().isEndRequested() ){
		if ( gState ){
			delete gState;
			gState = 0;
		}
		return;
	}
	//最初のフレームは初期化。最初の状態を描画して終わり。
	if ( !gState ){ 
		File file( "stageData.txt" );
		if ( !( file.getData() ) ){ //データない！
			cout << "stage file could not be read." << endl;
			return;
		}
		gState = new State( file.getData(), file.getSize() );
		//初回描画
		gState->draw();
		return; //そのまま終わる
	}
	bool cleared = false;
	//メインループ
	//クリアチェック
	if ( gState->hasCleared() ){
		cleared = true;
	}
	//入力取得
	cout << "a:left s:right w:up z:down. command?" << endl; //操作説明
	char input;
	cin >> input;
	//終了判定
	if ( input == 'q' ){
		delete gState;
		gState = 0;
		Framework::getInstance().requestEnd();
		return;
	}
	//更新
	gState->update( input );
	//描画
	gState->draw();

	if ( cleared ){
		//祝いのメッセージ
		cout << "Congratulation! you win." << endl;
		delete gState;
		gState = 0;
	}
}


