#include <sstream>
#include "GameLib/Framework.h"
using namespace GameLib;

using namespace std;

#include "Pad.h"
#include "Robo.h"
#include "Stage.h"
#include "Library/Matrix44.h"
#include "Library/Matrix34.h"

Robo* gRobo[ 2 ];
Stage* gStage;
int gTime;
bool gPlaying = false;
const int TIME_LIMIT = 90; //90秒

//ユーザ実装関数。中身はmainLoop()に
namespace GameLib{
	void Framework::update(){
		if ( !gStage ){
			setFrameRate( 60 );
			Pad::create();
			gRobo[ 0 ] = new Robo( 0 );
			gRobo[ 1 ] = new Robo( 1 );
			gRobo[ 0 ]->setPosition( Vector3( 0.0, 0.0, 50.0 ) );
			gRobo[ 0 ]->setAngleY( 180.0 );
			gStage = new Stage();
			gPlaying = true;
		}
		if ( gPlaying ){
			gRobo[ 0 ]->update( gRobo[ 1 ] );
			gRobo[ 1 ]->update( gRobo[ 0 ] );
			++gTime;
		}

		//0番ロボからカメラ行列ゲット
		Matrix44 pvm;
		pvm.setPerspectiveTransform( 45.0, 
			static_cast< double >( width() ),
			static_cast< double >( height() ),
			1.0,
			10000.0 );
		Matrix34 vm;
		gRobo[ 0 ]->getViewMatrix( &vm ); //ビュー行列はロボに作ってもらう
		pvm *= vm; 
		//描画
		gStage->draw( pvm );
		gRobo[ 0 ]->draw( pvm );
		gRobo[ 1 ]->draw( pvm );

		//とりあえずヒットポイントを表示
		int hp0 = gRobo[ 0 ]->hitPoint();
		int hp1 = gRobo[ 1 ]->hitPoint();
		int e0 = gRobo[ 0 ]->energy();
		int e1 = gRobo[ 1 ]->energy();
		bool lockOn0 = gRobo[ 0 ]->isLockOn();
		bool lockOn1 = gRobo[ 1 ]->isLockOn();
		ostringstream oss;
		oss << "ROBO 0 : HitPoint = " << hp0 << " Energy = " << e0 << " LockOn = " << lockOn0;
		drawDebugString( 0, 0, oss.str().c_str() );
		oss.str( "" );
		oss << "ROBO 1 : HitPoint = " << hp1 << " Enery = " << e1 << " LockOn = " << lockOn1;
		drawDebugString( 0, 1, oss.str().c_str() );
		oss.str( "" );
		oss << "TIME : " << ( 60 * TIME_LIMIT - gTime ) / 60;
		drawDebugString( 0, 2, oss.str().c_str() );
		
		if ( hp0 <= 0 || hp1 <= 0 || gTime >= 60 * TIME_LIMIT ){
			gPlaying = false;
			drawDebugString( 20, 5, "Game Over. press FIRE" );
			if ( Pad::instance()->isTriggered( Pad::FIRE, 0 ) ){
				SAFE_DELETE( gRobo[ 0 ] );
				SAFE_DELETE( gRobo[ 1 ] );
				gRobo[ 0 ] = new Robo( 0 );
				gRobo[ 1 ] = new Robo( 1 );
				gRobo[ 0 ]->setPosition( Vector3( 0.0, 0.0, 50.0 ) );
				gRobo[ 0 ]->setAngleY( 180.0 );
				gPlaying = true;
				gTime = 0;
			}
		}
		//終了判定(マウスで×ボタンが叩かれたか)
		if ( isEndRequested() ){
			if ( gStage ){
				Pad::destroy();
				SAFE_DELETE( gRobo[ 0 ] );
				SAFE_DELETE( gRobo[ 1 ] );
				SAFE_DELETE( gStage );
			}
		}
	}
}
