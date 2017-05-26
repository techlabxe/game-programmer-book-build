#include "GameLib/Framework.h"
#include "GameLib/Scene/Container.h"
#include "GameLib/Scene/Model.h"
#include "GameLib/Scene/Tree.h"
#include "GameLib/Scene/Animation.h"
#include "GameLib/WindowCreator/WindowCreator.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Math/Random.h"
#include "GameLib/Graphics/Manager.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Mouse.h"
#include "GameLib/Input/Keyboard.h"

using namespace GameLib::Scene;
using namespace GameLib::Graphics;
using namespace GameLib;

Container gContainer;
Array< Model > gModels;
Array< Tree > gTrees;
int gCount;
bool gFirst = true;
Vector3 gEyeTarget( 0.f, 0.f, 0.f );
float gEyeDistance = 1.f;
float gAngleY = 0.f;
float gAngleX = 0.f;
float gAngleZ = 0.f;
int gAnimationIndex = 0;

void load( const char* filename ){
	gAnimationIndex = 0;
	gModels.clear(); //モデルがあれば破棄
	gTrees.clear(); //ツリーがあれば破棄
	gContainer = Container::create( filename ); //ロードしなおし
	while ( !gContainer.isReady() ){ //ロード待ち
		;
	}

	int n = gContainer.treeNumber();
	gTrees.setSize( n );
	for ( int i = 0; i < n; ++i ){
		gTrees[ i ] = gContainer.createTree( i );
		if ( gContainer.animationNumber() > 0 ){
			gTrees[ i ].setAnimation( gContainer.animation( 0 ) );
		}
	}
	if ( n == 0 ){ //ツリーがあればモデルは出さない
		n = gContainer.batchNumber();
		gModels.setSize( n );
		for ( int i = 0; i < n; ++i ){
			gModels[ i ] = gContainer.createModel( i );
		}
	}
}

namespace GameLib{
	void Framework::configure( Configuration* c ){
		c->enableDragAndDrop();
		c->enableVSync();
		c->setWidth( 1280 );
		c->setHeight( 720 );
	}
	void Framework::update(){
		WindowCreator wc = WindowCreator::instance();
		if ( gFirst ){
			const char* filename = wc.commandLineString();
			if ( filename && filename[ 0 ] != '\0' ){
				load( filename );
			}
			gFirst = false;
		}else{
			//ドラッグアンドドロップを処理する
			int dropN = wc.droppedItemNumber();
			if ( dropN > 0 ){
				const char* filename = wc.droppedItem( 0 ); //0番以外無視
				load( filename );
				wc.clearDroppedItem(); //これを呼ぶとfilenameもこわれるので最後に。
			}
		}
		
		//カメラ入力反映
		Input::Manager im = Input::Manager::instance();
		Input::Mouse mouse = im.mouse();
		Input::Keyboard keyboard = im.keyboard();
		if ( mouse.isOn( Input::Mouse::BUTTON_MIDDLE ) ){
			Graphics::Manager().captureScreen( "capture.tga" );
		}
		//ビュー行列を作ろう
		Vector3 eyePosition = gEyeTarget;
		eyePosition.z += gEyeDistance;

		Matrix34 rm;
		rm.setRotationY( gAngleY );
		rm.rotateX( gAngleX );
		Vector3 tv( 0.f, 0.f, 1.f );
		rm.mul( &tv, tv );
		eyePosition.setMadd( gEyeTarget, tv, gEyeDistance );
		Matrix34 zrm;
		zrm.setRotationZ( gAngleZ );
		Vector3 up( 0.f, 1.f, 0.f );
		zrm.mul( &up, up );

		Matrix34 vm;
		vm.setViewTransform( eyePosition, gEyeTarget, up );
		if ( gContainer ){
			float x = static_cast< float >( mouse.velocityX() );
			float y = static_cast< float >( mouse.velocityY() );
			if ( mouse.isOn( Input::Mouse::BUTTON_LEFT ) && mouse.isOn( Input::Mouse::BUTTON_RIGHT ) ){ //両ボタンでZ回転
				gAngleZ -= 0.2f * x;
				gAngleZ -= 0.2f * y;
			}else if ( mouse.isOn( Input::Mouse::BUTTON_LEFT ) ){ //左ボタン回転
				gAngleX -= 0.2f * y;
				if ( gAngleX > 89.9f ){
					gAngleX = 89.9f;
				}else if ( gAngleX < -89.9f ){
					gAngleX = -89.9f;
				}
				gAngleY -= 0.5f * x;
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
		}
		//透視変換行列
		Matrix44 pm;
		pm.setPerspectiveTransform( 
			60.f, 
			static_cast< float >( width() ),
			static_cast< float >( height() ),
			gEyeDistance * 0.01f, gEyeDistance * 10.f );
		//次にPVを作る
		pm *= vm;
	
		if ( keyboard.isOn( 'G' ) ){
			gAngleX = gAngleY = gAngleZ = 0.f;
			gEyeTarget = 0.f;
		}

		//ライトでもうごかそか
		Graphics::Manager gm = Graphics::Manager::instance(); 
		gm.setProjectionViewMatrix( pm );
		gm.setLightingMode( LIGHTING_PER_PIXEL );
		gm.enableDepthTest( true );
		gm.enableDepthWrite( true );
		gm.setLightColor( 0, Vector3( 1.f, 1.f, 1.f ) ); //白
		gm.setLightColor( 1, Vector3( 1.f, 0.7f, 0.7f ) ); //赤
		gm.setLightColor( 2, Vector3( 0.7f, 1.f, 0.7f ) ); //緑
		gm.setLightColor( 3, Vector3( 0.7f, 0.7f, 1.f ) ); //青
		gm.setAmbientColor( Vector3( 0.2f, 0.2f, 0.2f ) );
		gm.setEyePosition( eyePosition );
		float t = gEyeDistance * 0.4f;
		float lightIntensity[ 4 ];
		for ( int i = 0; i < 4; ++i ){
			lightIntensity[ i ] = t;
		}
		Vector3 lightPositions[ 4 ];
		for ( int i = 0; i < 4; ++i ){
			float t = static_cast< float >( gCount * ( i + 1 ) ) / 5.f;
			float d = gEyeDistance * 2.f;
			lightPositions[ i ].set( sin( t )*cos( t ) * d,  sin( t )*sin( t ) * d, cos( t ) * d );
			lightPositions[ i ] += gEyeTarget;
		}
		for ( int i = 0; i < 4; ++i ){
			gm.setLightPosition( i, lightPositions[ i ] );
			gm.setLightIntensity( i, lightIntensity[ i ] );
		}
		for ( int i = 0; i < gModels.size(); ++i ){
			gModels[ i ].draw();
		}
		//アニメ切り替え
		if ( keyboard.isTriggered( ' ' ) ){
			if ( gContainer.animationNumber() > 0 ){
				++gAnimationIndex;
				if ( gAnimationIndex >= gContainer.animationNumber() ){
					gAnimationIndex = 0;
				}
				for ( int i = 0; i < gTrees.size(); ++i ){
					gTrees[ i ].setAnimation( gContainer.animation( gAnimationIndex ) );
				}
			}
		}
		for ( int i = 0; i < gTrees.size(); ++i ){
			gTrees[ i ].updateAnimation();
			gTrees[ i ].draw();
		}
		if ( isEndRequested() ){
			gModels.clear();
			gTrees.clear();
			gContainer.release();
		}


		++gCount;
	}
}
