#include <sstream>
#include "GameLib/Framework.h"
#include "GameLib/Math.h"
using namespace GameLib;

#include "Pad.h"
#include "Robo.h"
#include "Stage.h"
#include "Library/Matrix44.h"
#include "Library/Matrix34.h"
#include "Library/Vector2.h"

Robo* gRobo[ 2 ];
Stage* gStage;
int gTime;
bool gPlaying = false;
const int TIME_LIMIT = 90; //90秒

Vector3 gLightVector;
Vector3 gLightColor( 1.0, 0.6, 0.2 );
Vector3 gAmbient( 0.2, 0.3, 0.4 );

void drawRect( Vector2* p, unsigned c1, unsigned c2 ){
	Framework f = Framework::instance();
	double p4[ 4 ][ 4 ];
	for ( int i = 0; i < 4; ++i ){
		p4[ i ][ 0 ] = p[ i ].x;
		p4[ i ][ 1 ] = p[ i ].y;
		p4[ i ][ 2 ] = 0.0; //zは0固定
		p4[ i ][ 3 ] = 1.0; //wは1固定
	}
	f.drawTriangle3DH( p4[ 0 ], p4[ 1 ], p4[ 2 ], 0, 0, 0, c1, c1, c2 );
	f.drawTriangle3DH( p4[ 3 ], p4[ 1 ], p4[ 2 ], 0, 0, 0, c2, c1, c2 );
}


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
		//ライト動かしてみよう
		gLightVector.x = GameLib::sin( gTime ) * GameLib::cos( gTime );
		gLightVector.y = GameLib::sin( gTime ) * GameLib::sin( gTime );
		gLightVector.z = GameLib::cos( gTime );
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
		gStage->draw( pvm, gLightVector, gLightColor, gAmbient );
		gRobo[ 0 ]->draw( pvm, gLightVector, gLightColor, gAmbient );
		gRobo[ 1 ]->draw( pvm, gLightVector, gLightColor, gAmbient );
		//以下フロントエンド描画

		//必要な情報を抜いて
		int hp0 = gRobo[ 0 ]->hitPoint();
		int hp1 = gRobo[ 1 ]->hitPoint();
		int e0 = gRobo[ 0 ]->energy();
		bool lockOn0 = gRobo[ 0 ]->isLockOn();

		//Zテスト無用。アルファブレンドON
		enableDepthTest( false );
		enableDepthWrite( false );
		setBlendMode( BLEND_LINEAR );

		Vector2 p[ 4 ];
		unsigned c1;
		unsigned c2;
		//体力バー背景
		p[ 0 ].set( -0.9, 0.95 );
		p[ 1 ].set( -0.9, 0.87 );
		p[ 2 ].set( 0.1, 0.95 );
		p[ 3 ].set( 0.1, 0.87 );
		c1 = c2 = 0xff406080;
		drawRect( p, c1, c2 );
		p[ 0 ].set( -0.9, 0.85 );
		p[ 1 ].set( -0.9, 0.82 );
		p[ 2 ].set( 0.1, 0.85 );
		p[ 3 ].set( 0.1, 0.82 );
		c1 = c2 = 0xff806040;
		drawRect( p, c1, c2 );
		//体力バー本体
		double length = static_cast< float >( hp0 ) / static_cast< float >( Robo::mMaxHitPoint );
		p[ 0 ].set( -0.9, 0.95 );
		p[ 1 ].set( -0.9, 0.87 );
		p[ 2 ].set( -0.9 + length, 0.95 );
		p[ 3 ].set( -0.9 + length, 0.87 );
		c1 = 0xff882244;
		c2 = 0xff88ccff;
		drawRect( p, c1, c2 );
		length = static_cast< float >( hp1 ) / static_cast< float >( Robo::mMaxHitPoint );
		p[ 0 ].set( -0.9, 0.85 );
		p[ 1 ].set( -0.9, 0.82 );
		p[ 2 ].set( -0.9 + length, 0.85 );
		p[ 3 ].set( -0.9 + length, 0.82 );
		c1 = 0xffff4422;
		c2 = 0xffffcc88;
		drawRect( p, c1, c2 );
		//武器エネルギー
		//背景
		p[ 0 ].set( -0.1, -0.7 );
		p[ 1 ].set( -0.1, -0.8 );
		p[ 2 ].set( 0.1, -0.7 );
		p[ 3 ].set( 0.1, -0.8 );
		c1 = c2 = 0x80404040;
		drawRect( p, c1, c2 );
		//本体
		setBlendMode( BLEND_ADDITIVE ); //加算にしてみようかな
		length = 0.2 * static_cast< float >( e0 ) / static_cast< float >( Robo::mMaxEnergy );
		p[ 0 ].set( -0.1, -0.7 );
		p[ 1 ].set( -0.1, -0.8 );
		p[ 2 ].set( -0.1 + length, -0.7 );
		p[ 3 ].set( -0.1 + length, -0.8 );
		c1 = 0x80ff0000;
		c2 = 0x80ffff00;
		drawRect( p, c1, c2 );

		//レーダー
		setBlendMode( BLEND_LINEAR );
		//背景
		p[ 0 ].set( 0.7, 0.7 );
		p[ 1 ].set( 0.7, 0.9 );
		p[ 2 ].set( 0.9, 0.7 );
		p[ 3 ].set( 0.9, 0.9 );
		c1 = c2 = 0x80404040;
		drawRect( p, c1, c2 );
		setBlendMode( BLEND_ADDITIVE ); //加算にしてみようか
		//自分
		Vector2 t;
		t.set( gRobo[ 0 ]->position()->x, gRobo[ 0 ]->position()->z ); //x.z
		t *= 0.002; //+-50のステージを0.2の大きさに縮小するんだから、0.2/100で0.002
		t += Vector2( 0.8, 0.8 ); //(0.8,0.8)が中心なのだから、それを足す
		p[ 0 ].set( t.x - 0.005, t.y - 0.005 );
		p[ 1 ].set( t.x - 0.005, t.y + 0.005 );
		p[ 2 ].set( t.x + 0.005, t.y - 0.005 );
		p[ 3 ].set( t.x + 0.005, t.y + 0.005 );
		c1 = c2 = 0xcc0080ff;
		drawRect( p, c1, c2 );
		//敵
		t.set( gRobo[ 1 ]->position()->x, gRobo[ 1 ]->position()->z ); //x.z
		t *= 0.002; //+-50のステージを0.2の大きさに縮小するんだから、0.2/100で0.002
		t += Vector2( 0.8, 0.8 ); //(0.8,0.8)が中心なのだから、それを足す
		p[ 0 ].set( t.x - 0.005, t.y - 0.005 );
		p[ 1 ].set( t.x - 0.005, t.y + 0.005 );
		p[ 2 ].set( t.x + 0.005, t.y - 0.005 );
		p[ 3 ].set( t.x + 0.005, t.y + 0.005 );
		c1 = c2 = 0xccff8000;
		drawRect( p, c1, c2 );

		//ロックオンマーク
		if ( lockOn0 ){
			setBlendMode( BLEND_ADDITIVE ); //加算にしてみようかな
			//敵ロボを座標変換してスクリーン座標に変換
			Vector3 t = *gRobo[ 1 ]->position();
			double p4[ 4 ];
			pvm.multiply( p4, t );
			//XY座標はwで割れば出る。
			double x = p4[ 0 ] / p4[ 3 ];
			double y = p4[ 1 ] / p4[ 3 ];
			//色は赤かなあ
			c1 = c2 = 0x80ff0000;
			//線4本でやろう。
			p[ 0 ].set( x - 0.01, y + 0.2 );
			p[ 1 ].set( x - 0.01, y + 0.1 );
			p[ 2 ].set( x + 0.01, y + 0.2 );
			p[ 3 ].set( x + 0.01, y + 0.1 );
			drawRect( p, c1, c2 );
			p[ 0 ].set( x - 0.01, y - 0.2 );
			p[ 1 ].set( x - 0.01, y - 0.1 );
			p[ 2 ].set( x + 0.01, y - 0.2 );
			p[ 3 ].set( x + 0.01, y - 0.1 );
			drawRect( p, c1, c2 );
			p[ 0 ].set( x - 0.2, y - 0.01 );
			p[ 1 ].set( x - 0.2, y + 0.01 );
			p[ 2 ].set( x - 0.1, y - 0.01 );
			p[ 3 ].set( x - 0.1, y + 0.01 );
			drawRect( p, c1, c2 );
			p[ 0 ].set( x + 0.2, y - 0.01 );
			p[ 1 ].set( x + 0.2, y + 0.01 );
			p[ 2 ].set( x + 0.1, y - 0.01 );
			p[ 3 ].set( x + 0.1, y + 0.01 );
			drawRect( p, c1, c2 );
		}
		//時間制限
		setBlendMode( BLEND_LINEAR ); //もどして
		length = 1.9 * static_cast< double >( 60 * TIME_LIMIT - gTime ) / static_cast< double >( 60 * TIME_LIMIT );
		p[ 0 ].set( -0.95, -0.95 );
		p[ 1 ].set( -0.95, -0.9 );
		p[ 2 ].set( 0.95, -0.95 );
		p[ 3 ].set( 0.95, -0.9 );
		c1 = c2 = 0x80404040;
		drawRect( p, c1, c2 );
		p[ 0 ].set( -0.95, -0.95 );
		p[ 1 ].set( -0.95, -0.9 );
		p[ 2 ].set( -0.95 + length, -0.95 );
		p[ 3 ].set( -0.95 + length, -0.9 );
		setBlendMode( BLEND_ADDITIVE ); //加算?
		c1 = 0x80ff8888;
		c2 = 0x8088ffff;
		drawRect( p, c1, c2 );

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
