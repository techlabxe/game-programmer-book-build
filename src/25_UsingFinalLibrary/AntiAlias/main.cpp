//--------マルチサンプルアンチエイリアススイッチ---
bool gMultiSampleAntiAlias = true; //これを書き換えてみよう

#include "GameLib/Framework.h"
#include "GameLib/DebugScreen.h"
#include "GameLib/Math/Vector2.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Graphics/Manager.h"
#include "GameLib/Graphics/Texture.h"
#include "GameLib/Graphics/VertexBuffer.h"
#include "GameLib/Graphics/Vertex.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/WindowCreator/WindowCreator.h"
using namespace GameLib::Math;
using namespace GameLib::Graphics;

VertexBuffer gVertexBuffer;
Texture gTexture;
int gCount;
GameLib::Graphics::TextureFilter gTextureFilter = GameLib::Graphics::TEXTURE_FILTER_POINT;

namespace GameLib{
	//Configurationクラスを使って初期設定してやろう。
	void Framework::configure( Configuration* conf ){
		conf->enableAntiAlias( gMultiSampleAntiAlias ); //ジャギーを軽減する処理の有無
		conf->enableVSync( true );
	}
	void Framework::update(){
		using namespace Graphics;
		//何度となく出てくるのでm一文字であることを許して欲しい
		Graphics::Manager m = Graphics::Manager::instance();
		if ( gCount == 0 ){ 
			//--------------頂点バッファを作る-----------------------
			gVertexBuffer = VertexBuffer::create( 3 ); //3頂点の頂点バッファ
			Vertex* v = gVertexBuffer.lock(); //書き込みポインタを得る
			//位置を入れる
			v[ 0 ].mPosition.set( -1.f, -1.f, 0.f );
			v[ 1 ].mPosition.set( 0.f, 1.f, 0.f );
			v[ 2 ].mPosition.set( 1.f, -0.8f, 0.f );
			for ( int i = 0; i < 3; ++i ){
				v[ i ].mColor = 0xffffffff;
			}
			//テクスチャ座標を入れる(適当)
			v[ 0 ].mUv.set( -2.f, 2.f );
			v[ 1 ].mUv.set( 0.f, -2.f );
			v[ 2 ].mUv.set( 2.f, -2.f );
			//書き終わったら閉じる
			gVertexBuffer.unlock( &v );

			//-------------テクスチャを作る---------------------
			gTexture = Texture::create( "robo.tga" );
			while ( !gTexture.isReady() ){
				; //ロード待ち
			}
		}
		//使うデータを指定する
		m.setVertexBuffer( gVertexBuffer );
		m.setTexture( gTexture );

		//行列は単位行列でいい(透視変換ビュー行列とワールドの二つが必要)
		Matrix44 pvm;
		pvm.setIdentity();
		m.setProjectionViewMatrix( pvm );
		Matrix34 wm;
		wm.setIdentity();
		m.setWorldMatrix( wm );
		//描画
		m.draw( 0, 1, PRIMITIVE_TRIANGLE );

		//以下はスペースキーでフィルタを切り替えるコード
		if ( Input::Manager::instance().keyboard().isTriggered( ' ' ) ){
			if ( gTextureFilter == TEXTURE_FILTER_POINT ){
				gTextureFilter = TEXTURE_FILTER_LINEAR;
			}else if ( gTextureFilter == TEXTURE_FILTER_LINEAR ){
				gTextureFilter = TEXTURE_FILTER_POINT;
			}
		}
		//テクスチャフィルタセット
		m.setTextureFilter( gTextureFilter );

		DebugScreen ds = DebugScreen::instance();
		if ( gTextureFilter == TEXTURE_FILTER_POINT ){
			ds << "TextureFilter : POINT" << endl;
		}else if ( gTextureFilter == TEXTURE_FILTER_LINEAR ){
			ds <<"TextureFilter : LINEAR" << endl;
		}
		if ( gMultiSampleAntiAlias ){
			ds << "MultiSampleAntiAlias : TRUE" << endl;
		}else{
			ds << "MultiSampleAntiAlias : FALSE" << endl;
		}
		++gCount;

		//後片付け 
		if ( isEndRequested() ){
			//グローバルに置くと勝手には消えないので解放
			gTexture.release();
			gVertexBuffer.release();
		}
	}
}
