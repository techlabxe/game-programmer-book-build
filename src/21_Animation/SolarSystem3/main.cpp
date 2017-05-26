#include "GameLib/Framework.h"

#include "Library/GraphicsDatabase.h"
#include "Library/Tree.h"
#include "Library/Node.h"
#include "Library/Vector3.h"
#include "Library/Matrix44.h"
#include "Library/Matrix34.h"
#include "GameLib/Math.h"
using namespace GameLib;

GraphicsDatabase* gDatabase;
Tree* gSolarSystem;
Node* gSun;
Node* gEarth;
Node* gMoonRevolution; //月公転(ダミー)

int gCount;
Vector3 gLightVector;
Vector3 gLightColor( 0.8, 0.7, 0.6 );
Vector3 gAmbient( 0.2, 0.3, 0.4 );
Vector3 gEyePosition( 0.0, 10.0, 20.0 );

namespace GameLib{
	void Framework::update(){
		if ( !gDatabase ){
			setFrameRate( 60 );
			gDatabase = new GraphicsDatabase( "solarSystem.txt" );
			gSolarSystem = gDatabase->createTree( "solarSystem" );
			/* 親子関係図。
			                        +- earth
			sun - earthTranslation -+
			                        +- moonRevolution - moon
			*/
			gSun = gSolarSystem->node( "sun" );
			gEarth = gSolarSystem->node( "earth" );
			gMoonRevolution = gSolarSystem->node( "moonRevolution" );
		}
		//カメラとライトを動かしてみる
		gLightVector.x = sin( gCount ) * cos( gCount );
		gLightVector.y = sin( gCount ) * sin( gCount );
		gLightVector.z = cos( gCount );
		//ビュー行列、透視変換行列
		Matrix44 pm;
		pm.setPerspectiveTransform( 45.0, width(), height(), 1.0, 10000.0 );
		Matrix34 vm;
		vm.setViewTransform( gEyePosition, Vector3( 0.0, 0.0, 0.0 ) );
		pm *= vm;

		//各ノードに変換セット
		gSun->setRotation( Vector3( 0.0, gCount * 100.0 / 365.0, 0.0 ) );
		gEarth->setRotation( Vector3( 0.0, gCount * 100.0 / 3.0, 0.0 ) );
		gMoonRevolution->setRotation( Vector3( 0.0, gCount * 100.0 / 30.0, 0.0 ) );

		gSolarSystem->draw(
			pm,
			gLightVector,
			gLightColor,
			gAmbient );

		++gCount;
		if ( isEndRequested() ){
			SAFE_DELETE( gSolarSystem );
			SAFE_DELETE( gDatabase );
		}
	}
}
