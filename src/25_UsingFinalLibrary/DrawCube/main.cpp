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
#include "GameLib/Graphics/IndexBuffer.h"
#include "GameLib/Graphics/Vertex.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
using namespace GameLib::Math;
using namespace GameLib::Graphics;

VertexBuffer gVertexBuffer;
IndexBuffer gIndexBuffer;
Texture gTexture;
int gCount;
GameLib::Graphics::LightingMode gLightingMode = GameLib::Graphics::LIGHTING_NONE;

namespace GameLib{
	//Configurationクラスを使って初期設定してやろう。
	void Framework::configure( Configuration* conf ){
		conf->setWidth( 640 );
		conf->setHeight( 480 );
		conf->forbidFullScreen( true );
		conf->enableFullScreen( false ); //フルスクリーンモード起動
		conf->enableVSync( false ); //ハードウェアによるフレームレート一定化
		conf->enableAntiAlias( false ); //ジャギーを軽減する処理の有無
		conf->setTitle( "DrawCube Sample" ); //タイトルバーの文字列
	}
	void Framework::update(){
		using namespace Graphics;
		//何度となく出てくるのでm一文字であることを許して欲しい
		Graphics::Manager m = Graphics::Manager::instance();
		if ( gCount == 0 ){ 
			m.setTextureFilter( TEXTURE_FILTER_POINT );
			//--------------頂点バッファを作る-----------------------
			gVertexBuffer = VertexBuffer::create( 8 ); //8頂点の頂点バッファ
			Vertex* v = gVertexBuffer.lock(); //書き込みポインタを得る
			//位置を入れる
			v[ 0 ].mPosition.set( -1.f, -1.f, -1.f );
			v[ 1 ].mPosition.set( -1.f, -1.f, 1.f );
			v[ 2 ].mPosition.set( -1.f, 1.f, -1.f );
			v[ 3 ].mPosition.set( -1.f, 1.f, 1.f );
			v[ 4 ].mPosition.set( 1.f, -1.f, -1.f );
			v[ 5 ].mPosition.set( 1.f, -1.f, 1.f );
			v[ 6 ].mPosition.set( 1.f, 1.f, -1.f );
			v[ 7 ].mPosition.set( 1.f, 1.f, 1.f );
			//法線を入れる。斜めに出ている法線にしてみる。長さはだいたい1
			v[ 0 ].mNormal.set( -0.57f, -0.57f, -0.57f );
			v[ 1 ].mNormal.set( -0.57f, -0.57f, 0.57f );
			v[ 2 ].mNormal.set( -0.57f, 0.57f, -0.57f );
			v[ 3 ].mNormal.set( -0.57f, 0.57f, 0.57f );
			v[ 4 ].mNormal.set( 0.57f, -0.57f, -0.57f );
			v[ 5 ].mNormal.set( 0.57f, -0.57f, 0.57f );
			v[ 6 ].mNormal.set( 0.57f, 0.57f, -0.57f );
			v[ 7 ].mNormal.set( 0.57f, 0.57f, 0.57f );
			//頂点カラーを入れる(ライティングの効果を見たいので真っ白
			for ( int i = 0; i < 8; ++i ){
				v[ i ].mColor = 0xffffffff;
			}
			//テクスチャ座標を入れる(適当)
			v[ 0 ].mUv.set( 0.f, 0.f );
			v[ 1 ].mUv.set( 0.f, 1.f );
			v[ 2 ].mUv.set( 1.f, 0.f );
			v[ 3 ].mUv.set( 1.f, 1.f );
			v[ 4 ].mUv.set( 0.f, 0.f );
			v[ 5 ].mUv.set( 0.f, 1.f );
			v[ 6 ].mUv.set( 1.f, 0.f );
			v[ 7 ].mUv.set( 1.f, 1.f );
			//書き終わったら閉じる
			gVertexBuffer.unlock( &v );

			//-------------インデクスバッファを作る--------------------
			//3インデクスで三角形一つなので、インデクスバッファのサイズは普通3の倍数になる
			gIndexBuffer = IndexBuffer::create( 36 ); //36頂点のインデクスバッファ
			unsigned short indices[ 36 ] = {
				0, 1, 3, 3, 2, 0,
				4, 6, 7, 7, 5, 4,
				0, 4, 5, 5, 1, 0,
				2, 3, 7, 7, 6, 2,
				7, 3, 1, 1, 5, 7,
				2, 6, 4, 4, 0, 2,
			};
			unsigned short* iBuffer = gIndexBuffer.lock();
			for ( int i = 0; i < 36; ++i ){
				iBuffer[ i ] = indices[ i ];
			}
			gIndexBuffer.unlock( &iBuffer );

			//-------------テクスチャを作る---------------------
			gTexture = Texture::create( "robo.tga" );
			while ( !gTexture.isReady() ){
				; //ロード待ち
			}
		}
		//使うデータを指定する
		m.setVertexBuffer( gVertexBuffer );
		m.setIndexBuffer( gIndexBuffer );
		m.setTexture( gTexture );

		//アルファブレンドなどなどを設定
		m.setBlendMode( BLEND_LINEAR );
		m.setCullMode( CULL_BACK ); //裏をけずる
		m.enableDepthTest( true );
		m.enableDepthWrite( true );
		m.setLightingMode( gLightingMode );

		//ワールド行列を設定する
		Matrix34 wm;
		wm.setRotationY( gCount * 0.09f );
		wm.rotateX( gCount * 0.08f );
		wm.rotateZ( gCount * 0.07f );
		//ビュー行列を設定する
		Matrix34 vm;
		Vector3 eyePosition( 
			4.f * sin( gCount * 0.01f ),
			0.f,
			4.f * cos( gCount * 0.01f ) );
		Vector3 eyeTarget( 0.f, 0.f, 0.f );
		Vector3 up( 0.f, 1.f, 0.f ); //「上」を指定するベクタ。これをいじって横に傾ける。
		vm.setViewTransform( eyePosition, eyeTarget, up ); 

		//投影行列を設定する
		Matrix44 pm;
		pm.setPerspectiveTransform( 
			45.f, //45度
			static_cast< float >( width() ),
			static_cast< float >( height() ),
			1.f,
			1000.f );
		
		//行列をハードウェアに渡す
		pm *= vm; //透視変換×ビュー変換
		m.setProjectionViewMatrix( pm ); //透視変換ビュー行列
		m.setWorldMatrix( wm ); //ワールド行列

		//ライティングの設定をする。適当に飛び回る光の球のイメージで
		Vector3 lightPositions[ 4 ];
		float lightIntensities[ 4 ];
		for ( int i = 0; i < 4; ++i ){
			//まずは位置
			float phase = 90.f * static_cast< float >( i );
			float t = gCount * 0.025f + phase;
			lightPositions[ i ].x = 10.f * sin( t ) * sin( t );
			lightPositions[ i ].y = 10.f * sin( t ) * cos( t );
			lightPositions[ i ].z = 10.f * cos( t );
			lightIntensities[ i ] = 5.f;
		}
		//光の色は白、赤、緑、赤、にしてやろう
		Vector3 lightColors[ 4 ];
		lightColors[ 0 ].set( 1.f, 1.f, 1.f );
		lightColors[ 1 ].set( 1.f, 0.f, 0.f );
		lightColors[ 2 ].set( 0.f, 1.f, 0.f );
		lightColors[ 3 ].set( 0.f, 0.f, 1.f );
		//アンビエントは地味に0.3くらいで
		Vector3 ambient( 0.3f, 0.3f, 0.3f );
		//後は物体側の特性。あまりじっても仕方ないので固定値で。いろいろいじって結果がどうなるか見てやろう
		Vector3 diffuseColor( 1.f, 1.f, 1.f );
		Vector3 specularColor( 1.f, 1.f, 1.f );
		//これを変えるとハイライトの鋭さが変わる。小さいほどざらざらした表面というイメージ
		float specularSharpness = 100.f;
		//カメラ位置の設定
		m.setEyePosition( eyePosition );
		//ではライトの情報をセットしてやろう
		m.setAmbientColor( ambient );
		m.setDiffuseColor( diffuseColor );
		m.setSpecularColor( specularColor );
		m.setSpecularSharpness( specularSharpness );
		for ( int i = 0; i < 4; ++i ){
			m.setLightPosition( i, lightPositions[ i ] );
			m.setLightColor( i, lightColors[ i ] );
			m.setLightIntensity( i, lightIntensities[ i ] );
		}
		//後は描画するだけだ
		//引数は順にインデクスバッファの何番目から使うか、いくつ描くか、何を描くか。
		m.drawIndexed( 0, 12, PRIMITIVE_TRIANGLE );
		//上の場合、「0番目から12個の三角形を描く」となる。使われるインデクスは36つだ。

		//以下はスペースキーでライティングを切り替えるコードで、どうでも良い。
		if ( Input::Manager::instance().keyboard().isTriggered( ' ' ) ){
			if ( gLightingMode == LIGHTING_NONE ){
				gLightingMode = LIGHTING_PER_VERTEX;
			}else if ( gLightingMode == LIGHTING_PER_VERTEX ){
				gLightingMode = LIGHTING_PER_PIXEL;
			}else if ( gLightingMode == LIGHTING_PER_PIXEL ){
				gLightingMode = LIGHTING_NONE;
			}
		}
		DebugScreen ds = DebugScreen::instance();
		if ( gLightingMode == LIGHTING_NONE ){
			ds << "Lighting : NONE ( SPACE KEY TO CHANGE )";
		}else if ( gLightingMode == LIGHTING_PER_VERTEX ){
			ds << "Lighting : PER VERTEX ( SPACE KEY TO CHANGE )";
		}else if ( gLightingMode == LIGHTING_PER_PIXEL ){
			ds << "Lighting : PER PIXEL ( SPACE KEY TO CHANGE )";
		}
		++gCount;

		//後片付け 
		if ( isEndRequested() ){
			//グローバルに置くと勝手には消えないので開放
			gTexture.release();
			gIndexBuffer.release();
			gVertexBuffer.release();
		}
	}
}
