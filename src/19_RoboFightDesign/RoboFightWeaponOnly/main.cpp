#include <sstream>
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Pad.h"
#include "Robo.h"
#include "Stage.h"
#include "Library/Matrix44.h"
#include "Library/Matrix34.h"

Robo* gRobo[ 2 ];
Stage* gStage;

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
		}
		gRobo[ 0 ]->update( *gRobo[ 1 ]->position() );
		gRobo[ 1 ]->update( *gRobo[ 0 ]->position() );

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
