#include "GameLib/Framework.h"
#include "GameLib/DebugScreen.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Random.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Scene/Container.h"
#include "GameLib/Scene/Model.h"
#include "GameLib/Scene/CollisionDetector.h"
#include "GameLib/Graphics/Manager.h"
#include "GameLib/Input/Mouse.h"

using namespace std;
using namespace GameLib;
using namespace GameLib::Math;
using namespace GameLib::Scene;

class Sphere{
public:
	float mRadius;
	Model mModel;
};

Sphere* gSpheres;
const int gSphereNumber = 1000;
Container gContainer;
CollisionDetector gDetector;

float gEyeDistance = 100.f;
float gAngleY = 0.f;
float gAngleX = 0.f;

namespace GameLib{
	void Framework::configure( Configuration* c ){
		c->enableVSync( false );
	}
	void Framework::update(){
		if ( !gSpheres ){
			gDetector = CollisionDetector::create( gSphereNumber );
			gContainer = Container::create( "cube.txt" );
			while ( !gContainer.isReady() ){ ; } //ロード待ち
			gSpheres = new Sphere[ gSphereNumber ];
			//初期配置
			Random ran = Random::create();
			for ( int i = 0; i < gSphereNumber; ++i ){
				Sphere& s = gSpheres[ i ];
				s.mModel = gContainer.createModel( 0 ); //モデル作成
				Vector3 p(
					ran.getFloat( -100.f, 100.f ),
					ran.getFloat( -100.f, 100.f ),
					ran.getFloat( -100.f, 100.f ) );
				s.mModel.setPosition( p );
				s.mRadius = 5.f;
			}
		}
		//速度初期化
		Array< Vector3 > velocities( gSphereNumber );
		for ( int i = 0; i < gSphereNumber; ++i ){
			//速度を原点への引力で初期化
			velocities[ i ].setMul( *gSpheres[ i ].mModel.position(), -0.01f );
		}
		//衝突検出
		//登録。戻り値はIDなので保存しておく。0から順に返る保障はない。
		Array< int > idTable( gSphereNumber );
		for ( int i = 0; i < gSphereNumber; ++i ){
			idTable[ i ] = gDetector.add( *gSpheres[ i ].mModel.position(), gSpheres[ i ].mRadius );
		}
		//検出を行う
		Array< CollisionDetector::Pair > results;

		unsigned t0 = time(); //時間を計っておく
		gDetector.detect( &results );
		unsigned t1 = time();

		//衝突結果に応じて速度を修正
		for ( int i = 0; i < results.size(); ++i ){
			const CollisionDetector::Pair& pair = results[ i ];
			int i0 = idTable[ pair.mId0 ];
			int i1 = idTable[ pair.mId1 ];
			Sphere& s0 = gSpheres[ i0 ];
			Sphere& s1 = gSpheres[ i1 ];
			const Vector3& p0 = *s0.mModel.position();
			const Vector3& p1 = *s1.mModel.position();
			Vector3 n; //衝突法線
			n.setSub( p1, p0 );
			float distance = n.length();
			if ( distance != 0.f ){
				n *= 1.f / distance;
			}else{
				n.set( 0.f, 1.f, 0.f ); //完全に合わさったらY=1方向に跳ね返す
			}
			n *= 1.f; //跳ね返し強度
			velocities[ i0 ] -= n;
			velocities[ i1 ] += n;
		}
		//位置更新
		for ( int i = 0; i < gSphereNumber; ++i ){
			Sphere& s = gSpheres[ i ];
			Vector3 p = *s.mModel.position();
			p += velocities[ i ];
			s.mModel.setPosition( p );
		}
		//画面表示
		DebugScreen sc;
		sc << "FRAMERATE: " << frameRate() << " HIT:" << results.size() << " TIME:" << t1 - t0;
//------以下本筋に関係ないコード------

		//透視変換
		Matrix44 pm;
		pm.setPerspectiveTransform( 
			60.f, 
			static_cast< float >( width() ),
			static_cast< float >( height() ),
			gEyeDistance * 0.01f, gEyeDistance * 10.f );
		//ビュー行列を作ろう
		Matrix34 rm;
		rm.setRotationY( gAngleY );
		rm.rotateX( gAngleX );
		Vector3 tv( 0.f, 0.f, 1.f );
		rm.mul( &tv, tv );
		Vector3 eyePosition;
		eyePosition.setMul( tv, gEyeDistance );
		pm.multiplyViewTransform( eyePosition, Vector3( 0.f ), Vector3( 0.f, 1.f, 0.f ) );

		//カメラ入力反映
		Input::Mouse mouse; //実はこれで使える
		float x = static_cast< float >( mouse.velocityX() );
		float y = static_cast< float >( mouse.velocityY() );
		if ( mouse.isOn( Input::Mouse::BUTTON_LEFT ) ){ //左ボタン回転
				gAngleX -= 0.1f * y;
				if ( gAngleX > 89.f ){
					gAngleX = 89.f;
				}else if ( gAngleX < -89.f ){
					gAngleX = -89.f;
				}
				gAngleY -= 0.1f * x;
		}
		int w = mouse.wheel();
		if ( w < 0 ){
			gEyeDistance *= 0.9f;
		}else if ( w > 0 ){
			gEyeDistance *= 1.1f;
		}

		//描画
		Graphics::Manager gm;
		gm.setProjectionViewMatrix( pm );
		gm.setLightingMode( Graphics::LIGHTING_NONE );
		gm.enableDepthWrite( true );
		gm.enableDepthTest( true );
		gm.setAmbientColor( Vector3( 0.0f, 0.0f, 0.0f ) );

		gm.setLightColor( 0, Vector3( 0.1f, 0.5f, 0.1f ) );
		gm.setLightPosition( 0, Vector3( 0.f, 0.f, 0.f ) );
		gm.setLightIntensity( 0, 100.f );

		gm.setLightColor( 1, Vector3( 0.6f, 0.4f, 0.2f ) );
		gm.setLightPosition( 1, Vector3( 5777.f, 5777.f, 5777.f ) );
		gm.setLightIntensity( 1, 10000.f );

		gm.setLightColor( 2, Vector3( 0.2f, 0.4f, 0.6f ) );
		gm.setLightPosition( 2, Vector3( -5777.f, -5777.f, -5777.f ) );
		gm.setLightIntensity( 2, 10000.f );

		gm.setEyePosition( eyePosition );

		for ( int i = 0;i < gSphereNumber; ++i ){
			gSpheres[ i ].mModel.draw();
		}

		if ( isEndRequested() ){
			gDetector.release();
			gContainer.release();
			SAFE_DELETE_ARRAY( gSpheres );
		}
	}
}
