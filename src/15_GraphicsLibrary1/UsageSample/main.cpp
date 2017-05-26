#include "GameLib/Framework.h"
#include "Library/Vector3.h"
#include "Library/Vector2.h"
#include "Library/Matrix44.h"
#include "Library/Model.h"
#include "Library/Batch.h"
#include "Library/IndexBuffer.h"
#include "Library/VertexBuffer.h"
#include "Library/Camera.h"
#include "GameLib/Math.h"
using namespace GameLib;

const Vector3 gPositions[ 4 ] = {
	Vector3( -10.0, -10.0, 0.0 ),
	Vector3( -10.0, 10.0, 0.0 ),
	Vector3( 10.0, -10.0, 0.0 ),
	Vector3( 10.0, 10.0, 0.0 ),
};

const Vector2 gUVs[ 4 ] = {
	Vector2( 0.0, 0.0 ),
	Vector2( 0.0, 1.0 ),
	Vector2( 1.0, 0.0 ), 
	Vector2( 1.0, 1.0 ),
};

const unsigned gIndices[ 6 ] = {
	0, 1, 2,
	3, 1, 2,
};

bool gFirstFrame = true;
VertexBuffer* gVertexBuffer;
IndexBuffer* gIndexBuffer;
GameLib::Texture* gTexture;
Batch* gBatch;
Model* gModel;
Camera* gCamera;
int gCount;

namespace GameLib{
	void Framework::update(){
		if ( gFirstFrame ){
			setFrameRate( 60 );
			gFirstFrame = false;
			//頂点バッファ生成
			gVertexBuffer = new VertexBuffer( 4 );
			for ( int i = 0; i < 4; ++i ){
				gVertexBuffer->setPosition( i, gPositions[ i ] );
				gVertexBuffer->setUV( i, gUVs[ i ] );
			}
			//インデクスバッファ生成
			gIndexBuffer = new IndexBuffer( 6 );
			for ( int i = 0; i < 6; ++i ){
				gIndexBuffer->setIndex( i, gIndices[ i ] );
			}
			//テクスチャ生成
			createTexture( &gTexture, "grid.tga" );
			//バッチ生成
			gBatch = new Batch( gVertexBuffer, gIndexBuffer, gTexture, BLEND_OPAQUE );
			//モデル生成
			gModel = new Model( gBatch );
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

		++gCount;

		//後始末。作った順番と逆に消すのが通の技だ。
		//使っている方を先に消して、使われているものは後に消すべきである。
		if ( isEndRequested() ){
			SAFE_DELETE( gCamera );
			SAFE_DELETE( gModel );
			SAFE_DELETE( gBatch );
			SAFE_DELETE( gIndexBuffer );
			SAFE_DELETE( gVertexBuffer );
			destroyTexture( &gTexture );
		}
	}
}
