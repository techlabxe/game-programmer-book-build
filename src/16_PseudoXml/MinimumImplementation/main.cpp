#include "GameLib/Framework.h"
#include "Library/Vector3.h"
#include "Library/Vector2.h"
#include "Library/Matrix44.h"
#include "Library/Model.h"
#include "Library/GraphicsDatabase.h"
#include "Library/Camera.h"
#include "PseudoXml/Document.h"
#include "PseudoXml/Element.h"
#include "PseudoXml/Attribute.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Math.h"
using namespace GameLib;

bool gFirstFrame = true;
GraphicsDatabase* gGraphicsDatabase;
Model* gModel;
Camera* gCamera;
int gCount;

namespace GameLib{
	void Framework::update(){
		sleep( 16 );
		if ( gFirstFrame ){
			gFirstFrame = false;
			//データベース読み込み
			gGraphicsDatabase = new GraphicsDatabase( "data.txt" );
			//モデルを作る
			gModel = gGraphicsDatabase->createModel( "batch" );
			//カメラ生成
			gCamera = new Camera();
		}
		//カメラ更新
		gCamera->setPosition( Vector3(
			sin( gCount ) * 20.0,
			10.0,
			cos( gCount ) * 20.0 ) );

		Matrix44 pvm;
		gCamera->createPerspectiveViewMatrix( &pvm );
		//モデルの回転更新
		gModel->setAngle( Vector3( gCount, gCount * 2, gCount * 3 ) );
		//描画
		gModel->draw( pvm );

		//偽xml書き出しテスト
		if ( GameLib::Input::Manager::instance().keyboard().isTriggered( ' ' ) ){
			using namespace PseudoXml;
			Document document;
			Element* root = document.root();
			//根っこに一個加える
			root->setChildNumber( 1 );
			Element* cameraData = root->child( 0 );
			cameraData->setName( "CameraData" );
			//データを二つ入れる
			cameraData->setChildNumber( 2 );
			Element* e = cameraData->child( 0 );
			e->setName( "Position" );
			//位置データ
			e->setAttributeNumber( 3 );
			e->attribute( 0 )->set( "x", gCamera->position()->x );
			e->attribute( 1 )->set( "y", gCamera->position()->y );
			e->attribute( 2 )->set( "z", gCamera->position()->z );
			//注視点データ
			e = cameraData->child( 1 );
			e->setName( "Target" );
			e->setAttributeNumber( 3 );
			e->attribute( 0 )->set( "x", gCamera->target()->x );
			e->attribute( 1 )->set( "y", gCamera->target()->y );
			e->attribute( 2 )->set( "z", gCamera->target()->z );
			//書き込め
			document.write( "camera.txt" );
		}
		++gCount;

		//後始末。作った順番と逆に消すのが通の技だ。
		//使っている方を先に消して、使われているものは後に消すべきである。
		if ( isEndRequested() ){
			SAFE_DELETE( gCamera );
			SAFE_DELETE( gModel );
			SAFE_DELETE( gGraphicsDatabase );
		}
	}
}
