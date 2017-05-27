#ifndef INCLUDED_GAMELIB_GRAPHICS_MANAGER_H
#define INCLUDED_GAMELIB_GRAPHICS_MANAGER_H

#include "GameLib/Graphics/Enum.h"

namespace GameLib{

namespace Math{
	class Matrix44;
	class Matrix34;
	class Vector2;
	class Vector3;
}
using namespace Math;
	
namespace Graphics{

class Texture;
class VertexBuffer;
class IndexBuffer;

class Manager{
public:
	Manager();
	static Manager instance();

	void setVertexBuffer( VertexBuffer );
	void setIndexBuffer( IndexBuffer );
	void setTexture( Texture );

	void enableDepthTest( bool );
	void enableDepthWrite( bool );
	void setBlendMode( BlendMode );
	void setCullMode( CullMode );
	void setTextureFilter( TextureFilter );
	///透視変換ビュー変換行列セット。
	void setProjectionViewMatrix( const Matrix44& );
	///ワールド変換行列セット
	void setWorldMatrix( const Matrix34& );
	///ビューポート設定
	void setViewport( int x, int y, int width, int height );
	///ビューポート取得(0でないものにだけ入れるので欲しいものだけポインタをセット)
	void getViewport( int* xOut, int* yOut, int* widthOut, int* heightOut );

	///設定したテクスチャ、頂点バッファ、インデクスバッファで描画
	void drawIndexed(
		int offset, 
		int primitiveNumber, 
		PrimitiveType = PRIMITIVE_TRIANGLE );
	///設定したテクスチャ、頂点バッファで描画
	void draw( 
		int offset, 
		int primitiveNumber, 
		PrimitiveType = PRIMITIVE_TRIANGLE );
	///全画面にテクスチャを貼り付け
	void blendToScreen( Texture );
	///フルスクリーン化
	void enableFullScreen( bool );
	///描画バッファ幅を返す
	int width() const;
	///描画バッファ高さを返す
	int height() const;
	///フルスクリーン時のウィンドウ幅を返す
	int fullScreenWindowWidth() const;
	///フルスクリーン時のウィンドウ高さを返す
	int fullScreenWindowHeight() const;
	///フレーム番号を返す
	unsigned frameId() const;
	///スクリーンショットを取る(拡張子でDDS,TGA自動判別)。保存タイミングは描画終了後。
	void captureScreen( const char* filename );

	//ライティング関係
	///ピクセルシェーダを有効化する
	void setLightingMode( LightingMode );
	///ライト位置
	void setLightPosition( int index, const Vector3& position );
	///ライト強度(100なら100mの距離で1の明るさになる。強度Iとして、明るさは「I/距離」で決まる)
	void setLightIntensity( int index, float intensity );
	///ライト色
	void setLightColor( int index, const Vector3& );
	///視点位置
	void setEyePosition( const Vector3& position );
	///ディフューズカラー
	void setDiffuseColor( const Vector3& );
	///スペキュラ色
	void setSpecularColor( const Vector3& );
	///発光色
	void setEmissionColor( const Vector3& );
	///アンビエント色
	void setAmbientColor( const Vector3& );
	///透明度設定
	void setTransparency( float );
	///スペキュラの鋭さ(大きいほど鋭い。数字の意味は別の本を読もう)
	void setSpecularSharpness( float );

	//以下知らなくていい何か
	static void create( 
		void* windowHandle,
		int width,
		int height,
		bool fullScreen,
		bool vSync,
		bool antiAlias );
	static void destroy();
	void beginDraw();
	void endDraw();
	bool canRender() const; //デバイスロストしていればfalseが返る。
	void restore(); //デバイスロストから復帰する
	void getPointerModifier( float* scale, Vector2* offset ) const;
};

} //namespace Graphics
} //namespace GameLib

#endif
