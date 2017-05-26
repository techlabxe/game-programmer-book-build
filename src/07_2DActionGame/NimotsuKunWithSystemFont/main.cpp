#include "GameLib/Framework.h"
using namespace GameLib;

#include "State.h"
#include "File.h"
#include "Image.h"
#include "Sequence/Parent.h"

//グローバル変数
Sequence::Parent* gRootSequence = 0; //根っこシーケンス
int gCounter = 0; //メインループを回った回数を数えるカウンタ

//ユーザ実装関数。中身はmainLoop()に丸投げ
namespace GameLib{
	void Framework::update(){
		if ( !gRootSequence ){
			gRootSequence = new Sequence::Parent();
		}
		//フレームレート調整
		setFrameRate( 60 ); //一回呼べばいいのだが面倒なので呼んでしまう。

		if ( gCounter % 60 == 0 ){ //60フレームに一回フレームレート表示
			cout << " FrameRate:" << frameRate() << endl;
		}
		++gCounter;

		gRootSequence->update();
		//終了判定(qが押されたか、マウスで×ボタンが叩かれたか)
		if ( isKeyOn( 'q' ) ){
			requestEnd();
		}
		if ( isEndRequested() ){
			SAFE_DELETE( gRootSequence );
		}
	}
}


