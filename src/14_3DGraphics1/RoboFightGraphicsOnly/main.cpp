#include <sstream>
#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "Vector3.h"
#include "Matrix34.h"
#include "Matrix44.h"
#include "Robo.h"
#include "Stage.h"
#include "Pad.h"
#include "GameLib/Math.h"
using namespace GameLib;
using namespace std;

bool gFirstFrame = true;
Robo* gRobo[ 2 ]; //ロボグローバルにおいて見た。マネしないでね。
Stage* gStage; //ステージグローバルにおいて見た。マネしないでね。
int gCount;
Vector3 gEyePosition( 0.0, 0.0, 1.0 );
Vector3 gEyeTarget( 0.0, 0.0, 0.0 );

namespace GameLib{
	void Framework::update(){
		if ( gFirstFrame ){
			setFrameRate( 60 );
			gFirstFrame = false;
			
			Pad::create(); //パッド初期化
			gRobo[ 0 ] = new Robo( 0 );
			gRobo[ 1 ] = new Robo( 1 );
			gStage = new Stage();
			//初期位置設定
			gRobo[ 0 ]->setPosition( Vector3( 0.0, 0.0, 50.0 ) );
			gRobo[ 1 ]->setPosition( Vector3( 0.0, 0.0, -50.0 ) );
			gRobo[ 0 ]->setAngleY( 180.0 );
			gRobo[ 1 ]->setAngleY( 0.0 );
		}

		//メインループ
		//視線のみ計算。Robo::update()に使うので。でも視点データは前のフレーム
		Vector3 eyeVector;
		eyeVector.setSub( gEyeTarget, gEyePosition );
		//----入力判定と移動----
		for ( int i = 0; i < 2; ++i ){
			gRobo[ i ]->update( *gRobo[ 1-i ]->position(), eyeVector );
		}
		//----描画----
		//ビュー変換行列生成
		Vector3 dir;
		gRobo[ 0 ]->getDirection( &dir );
		gEyePosition = *( gRobo[ 0 ]->position() ); //まずロボの位置
		//長さを6メートルにしたdirを引く
		Vector3 t;
		t.setMul( dir, 6.0 );
		gEyePosition -= t;
		gEyePosition.y += 4.0; //6メートル上から見てやろう
		//注視点は逆に、8メートルにしたものを足す。
		gEyeTarget = *( gRobo[ 0 ]->position() );
		t.setMul( dir, 8.0 );
		gEyeTarget += t;
		Matrix34 vm;
		vm.setViewTransform( gEyePosition, gEyeTarget );

		//透視変換行列生成
		//透視変換行列
		Matrix44 pm; //60度でいいだろう。
		pm.setPerspectiveTransform( 60.0, 640.0, 480.0, 1.0, 1000.0 );
		//乗算
		pm *= vm;
		//描画
		gStage->draw( pm );
		for ( int i = 0; i < 2; ++i ){
			gRobo[ i ]->draw( pm );
		}
		++gCount;
		if ( isEndRequested() ){
			SAFE_DELETE( gRobo[ 0 ] );
			SAFE_DELETE( gRobo[ 1 ] );
			SAFE_DELETE( gStage );
			Pad::destroy();
		}
	}
}
