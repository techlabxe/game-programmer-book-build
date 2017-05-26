#include "GameLib/Framework.h"
#include "GameLib/DebugScreen.h"
#include "GameLib/Scene/CollisionMesh.h"
#include "GameLib/Scene/Container.h"
#include "GameLib/Scene/Model.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Graphics/Manager.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Mouse.h"
using namespace GameLib;
using namespace GameLib::Math;
using namespace GameLib::Scene;
using namespace std;

const char* file = "skyNtile.x.txt";

Container gContainer0;
Container gContainer1;
CollisionMesh gCollisionMesh;
Model gModel0;
Model gModel1;
Vector3 gPos;
float gEyeDistance = 2.f;
float gAngleY = 0.f;
float gAngleX = 0.f;
Vector3 gEyeTarget( 0.f, 0.f, 0.f );

namespace GameLib{
	void Framework::configure( Configuration* c ){
		c->enableVSync();
	}
	void Framework::update(){
		if ( !gContainer0 ){
			gContainer0 = Container::create( "cube.txt" );
			gContainer1 = Container::create( file );
			gCollisionMesh = CollisionMesh::create( file );
			while ( !gContainer0.isReady() || !gContainer1.isReady() || !gCollisionMesh.isReady() ){ ; }
			gModel0 = gContainer0.createModel( 0 );
			gModel0.setScale( Vector3( 0.1f ) );
			gModel0.setColor( Vector3( 1.f, 0.f, 0.f ) );
			gModel1 = gContainer1.createModel( 0 );
		}

		//ビュー行列を作ろう
		Matrix34 rm;
		rm.setRotationY( gAngleY );
		rm.rotateX( gAngleX );
		Vector3 tv( 0.f, 0.f, 1.f );
		rm.mul( &tv, tv );
		Vector3 eyePosition = gEyeTarget;
		eyePosition.madd( tv, gEyeDistance );
		Matrix34 vm;
		vm.setViewTransform( eyePosition, gEyeTarget, Vector3( 0.f, 1.f, 0.f ) );
		//透視変換
		Matrix44 pm;
		pm.setPerspectiveTransform( 
			60.f, 
			static_cast< float >( width() ),
			static_cast< float >( height() ),
			gEyeDistance * 0.01f, gEyeDistance * 10.f );
		pm *= vm;

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
		}else if ( mouse.isOn( Input::Mouse::BUTTON_RIGHT ) ){ //右ボタン、注視点移動
			Vector3 xv( vm.m00, vm.m01, vm.m02 );
			xv *= x;
			Vector3 yv( vm.m10, vm.m11, vm.m12 );
			yv *= y;
			gEyeTarget.madd( xv, -0.003f * gEyeDistance );
			gEyeTarget.madd( yv, 0.003f * gEyeDistance );
		}
		int w = mouse.wheel();
		if ( w < 0 ){
			gEyeDistance *= 0.9f;
		}else if ( w > 0 ){
			gEyeDistance *= 1.1f;
		}

		Graphics::Manager gm;
		gm.enableDepthTest( true );
		gm.enableDepthWrite( true );
		gm.setProjectionViewMatrix( pm );
		gm.setLightingMode( Graphics::LIGHTING_PER_PIXEL );
		gm.setAmbientColor( Vector3( 0.5f, 0.5f, 0.5f ) );
		gm.setEyePosition( eyePosition );
		gm.setLightColor( 0, Vector3( 1.f, 1.f, 1.f ) );
		gm.setLightPosition( 0, Vector3( 5777.f, 5777.f, 5777.f ) );
		gm.setLightIntensity( 0, 10000.f );

		Input::Keyboard kb;
		if ( kb.isOn( 'a' ) ){
			gPos.x -= 0.002f;
		}else if ( kb.isOn( 's' ) ){
			gPos.x += 0.002f;
		}
		if ( kb.isOn( 'w' ) ){
			gPos.z -= 0.002f;
		}else if ( kb.isOn( 'z' ) ){
			gPos.z += 0.002f;
		}
		//コリジョン。地面の高さゲット
		float t;
		Vector3 n( 0.f );
		gPos.y = 500.f;
		if ( gCollisionMesh.getIntersection( &t, &n, gPos, Vector3( 0.f, -1000.f, 0.f ) ) ){
			gPos.y = 500.f - t * 1000.f;
		}else{ 
			gPos.y = 0.f;
		}
		gModel0.setPosition( gPos );
		gModel0.draw();
		gModel1.draw();

		//視点から投げてみる
		float t2;
		Vector3 n2( 0.f );
		Vector3 p2( 0.f );
		Vector3 eye;
		eye.setSub( gEyeTarget, eyePosition );
		if ( gCollisionMesh.getIntersection( &t2, &n2, eyePosition, eye ) ){
			p2.setMadd( eyePosition, eye, t2 );
		}

		{
		typedef DebugScreen::Color C;
		DebugScreen sc;
		sc.precision( 3 );
		sc << gPos.x << "," << gPos.y << "," << gPos.z << endl;
		sc << n.x << "," << n.y << "," << n.z << endl;

		sc.setPosition( 40, 0 );
		sc << C( 0xff0000 ) << p2.x << "," << p2.y << "," << p2.z << C( 0xffffff ) << endl;
		sc << n2.x << "," << n2.y << "," << n2.z << endl;
		}

		if ( isEndRequested() ){
			gContainer0.release();
			gContainer1.release();
			gCollisionMesh.release();
			gModel0.release();
			gModel1.release();
		}
	}
}
