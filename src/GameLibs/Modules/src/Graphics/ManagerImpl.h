#ifndef INCLUDED_GAMELIB_GRAPHICS_MANAGERIMPL_H
#define INCLUDED_GAMELIB_GRAPHICS_MANAGERIMPL_H

//コード化されたシェーダオブジェクト
#include "Graphics/NoLightingVsGenerated.h"
#include "Graphics/VertexLightingVsGenerated.h"
#include "Graphics/PixelLightingVsGenerated.h"
#include "Graphics/PixelLightingPsGenerated.h"

#include "GameLib/Math/Functions.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Graphics/Enum.h"
#include "GameLib/Graphics/Texture.h"
#include "GameLib/Graphics/VertexBuffer.h"
#include "GameLib/Graphics/IndexBuffer.h"
#include "GameLib/Threading/Manager.h"
#include "Graphics/TextureImpl.h"
#include "Graphics/VertexBufferImpl.h"
#include "Graphics/IndexBufferImpl.h"

namespace GameLib{
using namespace Math;
namespace Graphics{

class ManagerImpl{
public:
	ManagerImpl( HWND wh, int w, int h, bool fullScreen, bool vSync, bool antiAlias ) :
	mDirect3d( 0 ),
	mDevice( 0 ),
	mWidth( w ),
	mHeight( h ),
	mFullScreen( fullScreen ),
	mVSync( vSync ),
	mPixelShaderReady( false ),
	mLightingMode( LIGHTING_NONE ),
	mFullScreenWindowWidth( 0 ),
	mFullScreenWindowHeight( 0 ),
	mFrameId( 0 ),
	mCanRender( true ),
	mWindowHandle( wh ),
	mFullScreenQuadVertexBuffer( 0 ),
	mVertexDeclaration( 0 ),
	mNoLightingVertexShader( 0 ),
	mVertexLightingVertexShader( 0 ),
	mPixelLightingVertexShader( 0 ),
	mPixelLightingPixelShader( 0 ),
	mWhiteTexture( 0 ),
	mCurrentTexture( 0 ),
	mCurrentVertexBuffer( 0 ),
	mCurrentIndexBuffer( 0 ),
	mCurrentDepthTest( TRUE ),
	mCurrentDepthWrite( TRUE ),
	mCurrentCullMode( D3DCULL_CW ),
	mCurrentAlphaBlend( FALSE ),
	mCurrentAlphaTest( FALSE ),
	mCurrentDestBlend( D3DBLEND_ZERO ),
	mCurrentMinFilter( D3DTEXF_POINT ),
	mCurrentMagFilter( D3DTEXF_POINT ),
	mCurrentMipFilter( D3DTEXF_NONE ),
	mBestMinFilter( D3DTEXF_POINT ),
	mBestMagFilter( D3DTEXF_POINT ),
	mBestMipFilter( D3DTEXF_NONE ),
	mMaxAnisotropy( 1 ),
	mMsaaQualityWindowed( 0 ),
	mMsaaQualityFullScreen( 0 ),
	mLightPositionXs( 0.f ),
	mLightPositionYs( 0.f ),
	mLightPositionZs( 0.f ),
	mEyePosition( 0.f, 0.f, 0.f ),
	mDiffuseColor( 1.f ),
	mSpecularColor( 1.f ),
	mEmissionColor( 0.f ),
	mAmbientColor( 1.f ),
	mLightChanged( true ),//初回のdrawはライト情報送ってくれ
	mMatricesChanged( true ){ //初回のdrawは行列も送ってくれ
		for ( int i = 0; i < 4; ++i ){
			mLightColors[ i ].set( 0.f, 0.f, 0.f );
			mLightIntensities[ i ] = 0.f;
		}
		mProjectionViewMatrix.setIdentity();
		mWorldMatrix.setIdentity();
		HRESULT hr;
		//Direct3D作成
		mDirect3d = Direct3DCreate9( D3D_SDK_VERSION );
		STRONG_ASSERT( mDirect3d && "Direct3D creation failed." );

		//capsゲット
		D3DCAPS9 caps;
		mDirect3d->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps );
		//頂点シェーダ1.1行けてる？
		bool vs11 = ( caps.VertexShaderVersion >= D3DVS_VERSION( 1, 1 ) );
		//ピクセル2.0行けてる？
		mPixelShaderReady = ( caps.PixelShaderVersion >= D3DPS_VERSION( 2, 0 ) );
		//アンイソ何倍いけるの?
		mMaxAnisotropy = caps.MaxAnisotropy;
		//最強テクスチャフィルタ取得
		DWORD tfCaps = caps.TextureFilterCaps;
		if ( tfCaps & D3DPTFILTERCAPS_MIPFLINEAR ){
			mBestMipFilter = D3DTEXF_LINEAR;
		}else if ( tfCaps & D3DPTFILTERCAPS_MIPFPOINT ){
			mBestMipFilter = D3DTEXF_POINT;
		}
		if ( tfCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC ){
			mBestMagFilter = D3DTEXF_ANISOTROPIC;
		}else if ( tfCaps & D3DPTFILTERCAPS_MAGFLINEAR ){
			mBestMagFilter = D3DTEXF_LINEAR;
		}
		if ( tfCaps & D3DPTFILTERCAPS_MINFANISOTROPIC ){
			mBestMinFilter = D3DTEXF_ANISOTROPIC;
		}else if ( tfCaps & D3DPTFILTERCAPS_MINFLINEAR ){
			mBestMinFilter = D3DTEXF_LINEAR;
		}
		//MSAA何倍行けるの？
		if ( antiAlias ){
			hr = mDirect3d->CheckDeviceMultiSampleType(
				D3DADAPTER_DEFAULT, 
				D3DDEVTYPE_HAL, 
				D3DFMT_X8R8G8B8,
				TRUE,
				D3DMULTISAMPLE_NONMASKABLE,
				&mMsaaQualityWindowed );
			STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "CheckDeviceMultiSampleType : INVALID CALL" );
			STRONG_ASSERT( hr != D3DERR_INVALIDDEVICE && "CheckDeviceMultiSampleType : INVALID CALL" );
			if ( FAILED( hr ) ){
				mMsaaQualityWindowed = 0;
			}
			hr = mDirect3d->CheckDeviceMultiSampleType(
				D3DADAPTER_DEFAULT, 
				D3DDEVTYPE_HAL, 
				D3DFMT_X8R8G8B8,
				TRUE,
				D3DMULTISAMPLE_NONMASKABLE,
				&mMsaaQualityFullScreen );
			STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "CheckDeviceMultiSampleType : INVALID CALL" );
			STRONG_ASSERT( hr != D3DERR_INVALIDDEVICE && "CheckDeviceMultiSampleType : INVALID CALL" );
			if ( FAILED( hr ) ){
				mMsaaQualityFullScreen = 0;
			}
		}
		//解像度列挙
		UINT modeCount = mDirect3d->GetAdapterModeCount( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8 );
		Array< D3DDISPLAYMODE > modes( modeCount );
		for ( UINT i = 0; i < modeCount; ++i ){
			hr = mDirect3d->EnumAdapterModes( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &modes[ i ] );
		}
		//現在のモードがモニタの自然なサイズと仮定し、そのアスペクト比以外は除外する。
		D3DDISPLAYMODE currentMode;
		hr = mDirect3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &currentMode );
		STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "GetAdapterDisplayMode : INVALID CALL" );
		float nativeAspect = static_cast< float >( currentMode.Width ) / static_cast< float >( currentMode.Height );
		//オリジナル解像度より大きくて近いのをまず探す
		int minScore = 0x7fffffff;
		int minScoreIndex = -1;
		for ( UINT i = 0; i < modeCount; ++i ){
			int tw = static_cast< int >( modes[ i ].Width );
			int th = static_cast< int >( modes[ i ].Height );
			float aspect = static_cast< float >( tw ) / static_cast< float >( th );
			if ( Math::abs( nativeAspect - aspect ) < 0.0001f ){ //アスペクト比が同じで近い奴
				if ( tw >= w && th >= h ){
					int score = ( tw - w ) + ( th - h );
					if ( score < minScore ){
						minScore = score;
						minScoreIndex = i;
					}
				}
			}
		}
		//なければ、小さくてもいいから差の小さいものを探す
		if ( minScoreIndex == -1 ){
			for ( UINT i = 0; i < modeCount; ++i ){
				int tw = static_cast< int >( modes[ i ].Width );
				int th = static_cast< int >( modes[ i ].Height );
				float aspect = static_cast< float >( tw ) / static_cast< float >( th );
				if ( Math::abs( nativeAspect - aspect ) < 0.0001f ){ //アスペクト比が同じで近い奴
					int score = std::abs( tw - w ) + std::abs( th - h );
					if ( score < minScore ){
						minScore = score;
						minScoreIndex = i;
					}
				}
			}
		}
		mFullScreenWindowWidth = modes[ minScoreIndex ].Width;
		mFullScreenWindowHeight = modes[ minScoreIndex ].Height;

		//パラメータ作成
		ZeroMemory( &mPresentParameters, sizeof( D3DPRESENT_PARAMETERS ) );
		mPresentParameters.BackBufferCount = 1;
		mPresentParameters.EnableAutoDepthStencil = TRUE;
		mPresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
		mPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		mPresentParameters.PresentationInterval = ( mVSync ) ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
		mPresentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
		//フルスクリーンとウィンドウで分岐
		if ( mFullScreen ){
			mPresentParameters.Windowed = FALSE;
			mPresentParameters.BackBufferWidth = mFullScreenWindowWidth; 
			mPresentParameters.BackBufferHeight = mFullScreenWindowHeight;
			if ( mMsaaQualityFullScreen > 0 ){
				mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
				mPresentParameters.MultiSampleQuality = mMsaaQualityFullScreen - 1;
			}else{
				mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
				mPresentParameters.MultiSampleQuality = 0;
			}
		}else{
			mPresentParameters.Windowed = TRUE;
			mPresentParameters.BackBufferWidth = w; 
			mPresentParameters.BackBufferHeight = h;
			if ( mMsaaQualityWindowed > 0 ){
				mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
				mPresentParameters.MultiSampleQuality = mMsaaQualityWindowed - 1;
			}else{
				mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
				mPresentParameters.MultiSampleQuality = 0;
			}
		}
		//HW頂点処理で試行
		if ( vs11 ){
			hr = mDirect3d->CreateDevice(
				D3DADAPTER_DEFAULT, 
				D3DDEVTYPE_HAL, 
				mWindowHandle, 
				D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
				&mPresentParameters,
				&mDevice);
		}else{
			hr = D3DERR_INVALIDCALL; //適当なエラーを入れておいて、次のソフト頂点初期化へ
		}
		if ( FAILED( hr ) ){ //ダメならソフトウェア頂点で
			hr = mDirect3d->CreateDevice(
				D3DADAPTER_DEFAULT, 
				D3DDEVTYPE_HAL, 
				mWindowHandle, 
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&mPresentParameters,
				&mDevice);
			if ( FAILED( hr ) ){
				STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "CreateDevice : INVALID CALL" );
				STRONG_ASSERT( hr != D3DERR_NOTAVAILABLE && "CreateDevice : NOT AVAILABLE" );
				STRONG_ASSERT( hr != D3DERR_OUTOFVIDEOMEMORY && "CreateDevice : OUT OF VIDEO MEMORY" );
				HALT( "CreateDevice : unknown error" );
			}
		}
		//頂点宣言を作る
		D3DVERTEXELEMENT9 vElements[] = {
			{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 0, 28, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			D3DDECL_END(),
		};
		hr = mDevice->CreateVertexDeclaration( vElements, &mVertexDeclaration );
		STRONG_ASSERT( SUCCEEDED( hr ) && "CreateVertexDeclaration : INVALID CALL" );

		//シェーダを作る。
		createShader(
			&mNoLightingVertexShader,
			0,
			gShaderNoLightingVsObj,
			sizeof( gShaderNoLightingVsObj ) );
		createShader(
			&mVertexLightingVertexShader,
			0,
			gShaderVertexLightingVsObj,
			sizeof( gShaderVertexLightingVsObj ) );
		//ピクセルシェーダを使う場合
		if ( mPixelShaderReady ){
			createShader(
				&mPixelLightingVertexShader,
				0,
				gShaderPixelLightingVsObj,
				sizeof( gShaderPixelLightingVsObj ) );
			createShader(
				0,
				&mPixelLightingPixelShader,
				gShaderPixelLightingPsObj,
				sizeof( gShaderPixelLightingPsObj ) );
		}

		//初期ビューポート設定(ただしこれはダミーで、実際には修正される)
		mViewport.X = mViewport.Y = 0;
		mViewport.Width = mWidth;
		mViewport.Height = mHeight;
		mViewport.MinZ = 0.f;
		mViewport.MaxZ = 1.f;

		//全画面描画用頂点バッファとインデクスバッファ
		mFullScreenQuadVertexBuffer = NEW VertexBuffer::Impl( 3, mDevice );
		Vertex* v = static_cast< Vertex* >( mFullScreenQuadVertexBuffer->lock() );
		for ( int i = 0; i < 3; ++i ){
			v[ i ].mColor = 0xffffffff;
			v[ i ].mPosition.set( -1.f, 1.f, 0.f, 1.f );
			v[ i ].mUv.set( 0.f, 0.f );
		}
		v[ 2 ].mPosition.x += 4.f; 
		v[ 1 ].mPosition.y -= 4.f;//yは逆
		v[ 2 ].mUv.x = 2.f;
		v[ 1 ].mUv.y = 2.f;
		mFullScreenQuadVertexBuffer->unlock();
		v = 0; //使い終わった

		//真っ白テクスチャ準備
		mWhiteTexture = NEW Texture::Impl( 1, 1, false, mDevice );
		unsigned* textureData;
		int pitch;
		mWhiteTexture->lock( &textureData, &pitch, 0 );
		*textureData = 0xffffffff;
		mWhiteTexture->unlock( 0 );
		textureData = 0;
		//起動時デバイス初期化
		setInitialStates();
	}
	~ManagerImpl(){
		setTexture( 0 );
		setVertexBuffer( 0 );
		setIndexBuffer( 0 );

		mWhiteTexture->release(); //直接Implをnewした時はrelease()してやらないと怒られる
		SAFE_DELETE( mWhiteTexture );
		mFullScreenQuadVertexBuffer->release();
		SAFE_DELETE( mFullScreenQuadVertexBuffer );

		mVertexDeclaration->Release();
		mVertexDeclaration = 0;
		mNoLightingVertexShader->Release();
		mNoLightingVertexShader = 0;
		mVertexLightingVertexShader->Release();
		mVertexLightingVertexShader = 0;
		if ( mPixelShaderReady ){
			mPixelLightingVertexShader->Release();
			mPixelLightingVertexShader = 0;
			mPixelLightingPixelShader->Release();
			mPixelLightingPixelShader = 0;
		}
		mDevice->Release();
		mDevice = 0;
		mDirect3d->Release(); 
		mDirect3d = 0;
	}
	void createShader(
	IDirect3DVertexShader9** outVs,
	IDirect3DPixelShader9** outPs,
	const char* shaderObjFile,
	int shaderObjSize ){
		DWORD* shaderObj = 0;

		//ライティングなし
		const unsigned char* shaderObjRaw;
		shaderObjRaw = reinterpret_cast< const unsigned char* >( shaderObjFile );
		STRONG_ASSERT( shaderObjSize % 4 == 0 );
		shaderObj = NEW DWORD[ shaderObjSize / 4 ];
		//DWORD配列へコピー
		for ( int i = 0; i < shaderObjSize / 4; ++i ){
			shaderObj[ i ] = shaderObjRaw[ i * 4 + 0];
			shaderObj[ i ] |= shaderObjRaw[ i * 4 + 1 ] << 8;
			shaderObj[ i ] |= shaderObjRaw[ i * 4 + 2 ] << 16;
			shaderObj[ i ] |= shaderObjRaw[ i * 4 + 3 ] << 24;
		}
		HRESULT hr;
		if ( outVs ){
			hr = mDevice->CreateVertexShader( shaderObj, outVs );
		}else{
			hr = mDevice->CreatePixelShader( shaderObj, outPs );
		}
		STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "CreateXXXShader : INVALID CALL" );
		STRONG_ASSERT( hr != D3DERR_OUTOFVIDEOMEMORY && "CreateXXXShader : OUTOF VIDEO MEMORY" );
		STRONG_ASSERT( hr != E_OUTOFMEMORY && "CreateXXXShader : E_OUTOFMEMORY" );
		SAFE_DELETE( shaderObj );
	}

	//起動時、DeviceLost後に呼ぶ。基本的に何もなければ一回でいいステート群
	void setInitialStates(){
		HRESULT hr;
		//頂点宣言セット
		hr = mDevice->SetVertexDeclaration( mVertexDeclaration );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetVertexDeclaration : INVALID CALL" );
		//シェーダセット
		setShader();
		//いじらないステートセット
		hr = mDevice->SetRenderState( D3DRS_ALPHAREF, 0x80 );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );
		hr = mDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );
		hr = mDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetTextureStageState : INVALID CALL" );
		hr = mDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetTextureStageState : INVALID CALL" );
		hr = mDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );
		hr = mDevice->SetSamplerState( 0, D3DSAMP_MAXANISOTROPY, mMaxAnisotropy );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );

		//再設定
		hr = mDevice->SetRenderState( D3DRS_CULLMODE, mCurrentCullMode );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );
		hr = mDevice->SetRenderState( D3DRS_ZENABLE, mCurrentDepthTest );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );
		hr = mDevice->SetRenderState( D3DRS_ZWRITEENABLE, mCurrentDepthWrite );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );
		hr = mDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, mCurrentAlphaBlend );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );
		hr = mDevice->SetRenderState( D3DRS_DESTBLEND, mCurrentDestBlend );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );
		hr = mDevice->SetRenderState( D3DRS_ALPHATESTENABLE, mCurrentAlphaTest );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );
		hr = mDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, mCurrentMinFilter );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetSamplerState : INVALID CALL" );
		hr = mDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, mCurrentMagFilter );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetSamplerState : INVALID CALL" );
		hr = mDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, mCurrentMipFilter );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetSamplerState : INVALID CALL" );

		if ( mCurrentTexture ){
			mCurrentTexture->release();
			mCurrentTexture = 0;
		}
		if ( mCurrentVertexBuffer ){
			mCurrentVertexBuffer->release();
			mCurrentVertexBuffer = 0;
		}
		if ( mCurrentIndexBuffer ){
			mCurrentIndexBuffer->release();
			mCurrentIndexBuffer = 0;
		}
		mLightChanged = true; //ライトおくってね
		mMatricesChanged = true; //行列おくってね
		//いじられうる標準値をセット
		setTextureFilter( TEXTURE_FILTER_LINEAR );
	}
	void setShader(){
		HRESULT hr;
		//シェーダセット
		if ( mLightingMode == LIGHTING_NONE ){
			hr = mDevice->SetVertexShader( mNoLightingVertexShader );
			STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "SetVertexShader : INVALID CALL" );
			hr = mDevice->SetPixelShader( 0 );
			STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "SetPixelShader : INVALID CALL" );
		}else if ( mLightingMode == LIGHTING_PER_VERTEX ){
			hr = mDevice->SetVertexShader( mVertexLightingVertexShader );
			STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "SetVertexShader : INVALID CALL" );
			hr = mDevice->SetPixelShader( 0 );
			STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "SetPixelShader : INVALID CALL" );
		}else if ( mLightingMode == LIGHTING_PER_PIXEL ){
			hr = mDevice->SetVertexShader( mPixelLightingVertexShader );
			STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "SetVertexShader : INVALID CALL" );
			hr = mDevice->SetPixelShader( mPixelLightingPixelShader );
			STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "SetPixelShader : INVALID CALL" );
		}
	}
	void beginDraw(){
		HRESULT hr = mDevice->BeginScene();
		STRONG_ASSERT( SUCCEEDED( hr ) && "BeginScene : DRIVER INTERNAL ERROR" );
		//まず一旦ビューポートを全面にして全体クリア
		D3DVIEWPORT9 viewport;
		viewport.X = viewport.Y = 0;
		viewport.Width = mPresentParameters.BackBufferWidth;
		viewport.Height = mPresentParameters.BackBufferHeight;
		viewport.MinZ = 0.f;
		viewport.MaxZ = 1.f;
		mDevice->SetViewport( &viewport );
		//クリアしとく
		hr = mDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.f, 0 );
		STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "Clear : INVALIDCALL" );
		//本物のビューポートを設定
		setViewport( mViewport.X, mViewport.Y, mViewport.Width, mViewport.Height );
	}
	void endDraw(){
		//キャプチャー処理
		if ( mCaptureRequest && ( mCaptureFilename.size() > 0 ) ){
			capture();
		}
		HRESULT hr;
		hr = mDevice->EndScene();
		STRONG_ASSERT( SUCCEEDED( hr ) && "EndScene : DRIVER INTERNAL ERROR" );
		//1コアマシンの場合、VSyncに1ms未満しかかかっていない場合は1ms寝る。他のスレッドに処理を回すため。
		if ( Threading::Manager().getCoreNumber() == 1 ){
			unsigned t0 = timeGetTime();
			hr = mDevice->Present( NULL, NULL, NULL, NULL );
			unsigned t1 = timeGetTime();
			if ( t1 == t0 ){
				Sleep( 1 );
			}
		}else{
			hr = mDevice->Present( NULL, NULL, NULL, NULL );
		}
		if ( FAILED( hr ) ){
			mCanRender = false;
			STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "Present : INVALID CALL" );
			STRONG_ASSERT( hr != D3DERR_DRIVERINTERNALERROR && "Present : DRIVER INTERNAL ERROR" );
		}
		++mFrameId; //フレーム番号インクリメント
	}
	void capture(){
		int w = mPresentParameters.BackBufferWidth;
		int h = mPresentParameters.BackBufferHeight;

		HRESULT hr;
		IDirect3DSurface9* srcSurface;
		hr = mDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &srcSurface );
		STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "GetBackBuffer : INVALID CALL" );
		//書き込み側
		IDirect3DSurface9* tmpSurface;
		hr = mDevice->CreateRenderTarget(
			w,
			h,
			D3DFMT_X8R8G8B8,
			D3DMULTISAMPLE_NONE,
			0,
			TRUE,
			&tmpSurface,
			NULL );
		STRONG_ASSERT( hr != D3DERR_NOTAVAILABLE && "CreateRenderTarget : NOT AVAILABLE" );
		STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "CreateRenderTarget : INVALID CALL" );
		STRONG_ASSERT( hr != D3DERR_OUTOFVIDEOMEMORY && "CreateRenderTarget : OUT OF VIDEO MEMORY" );
		STRONG_ASSERT( hr != E_OUTOFMEMORY && "CreateRenderTarget : OUT OF MEMORY" );
		//MSAAはずし
		hr = mDevice->StretchRect( srcSurface, NULL, tmpSurface, NULL, D3DTEXF_POINT );
		STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "StretchRect : INVALID CALL" );
		//メモリ側のサーフィスを作成
		IDirect3DSurface9* dstSurface;
		hr = mDevice->CreateOffscreenPlainSurface( w, h, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &dstSurface, NULL );
		//メモリにデータ転送
		hr = mDevice->GetRenderTargetData( tmpSurface, dstSurface );
		//次、ロックして読み出しつつ書き込み
		D3DLOCKED_RECT rect;
		hr = dstSurface->LockRect( &rect, NULL, D3DLOCK_READONLY );
		STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "LockRect : INVALID CALL" );
		Texture::Impl::write( mCaptureFilename.c_str(), w, h, rect.Pitch, static_cast< const unsigned* >( rect.pBits ) );
		hr = dstSurface->UnlockRect();
		STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "UnlockRect : INVALID CALL" );
		//後始末
		dstSurface->Release();
		dstSurface = 0;
		tmpSurface->Release();
		tmpSurface = 0;
		srcSurface->Release();
		srcSurface = 0;
		mCaptureRequest = false;
	}
	void restore(){
		if ( mCanRender ){
			return; //リセットの必要なし
		}
		HRESULT hr = mDevice->TestCooperativeLevel();
		STRONG_ASSERT( hr != D3DERR_DRIVERINTERNALERROR && "TestCooperativeLevel : DRIVER INTERNAL ERROR" );
		if ( hr == D3D_OK || hr == D3DERR_DEVICENOTRESET ){ //リセットするぜ
			//フルスクリーンとウィンドウで分岐
			if ( mFullScreen ){
				mPresentParameters.Windowed = FALSE;
				mPresentParameters.BackBufferWidth = mFullScreenWindowWidth; 
				mPresentParameters.BackBufferHeight = mFullScreenWindowHeight;
				if ( mMsaaQualityFullScreen > 0 ){
					mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
					mPresentParameters.MultiSampleQuality = mMsaaQualityFullScreen - 1;
				}else{
					mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
					mPresentParameters.MultiSampleQuality = 0;
				}
			}else{
				mPresentParameters.Windowed = TRUE;
				mPresentParameters.BackBufferWidth = mWidth; 
				mPresentParameters.BackBufferHeight = mHeight;
				if ( mMsaaQualityWindowed > 0 ){
					mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
					mPresentParameters.MultiSampleQuality = mMsaaQualityWindowed - 1;
				}else{
					mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
					mPresentParameters.MultiSampleQuality = 0;
				}
			}
			hr = mDevice->Reset( &mPresentParameters );
			STRONG_ASSERT( hr != D3DERR_DRIVERINTERNALERROR && "Reset : DRIVER INTERNAL ERROR" );
			STRONG_ASSERT( hr != D3DERR_OUTOFVIDEOMEMORY && "Reset : OUT OF VIDEO MEMORY" );
			if ( SUCCEEDED( hr ) ){
				setInitialStates(); //デバイスが飛んだので一部再設定
				mCanRender = true; //おーけー
				cout << "Graphics-restore() : Direct3D Device Reset Succeeded" << endl;
			}else{
				cout << "Graphics-restore() : Direct3D Device Reset Failed" << endl;
			}
		}
	}
	void setTexture( Texture::Impl* o ){
		if ( mCurrentTexture == o ){
			return;
		}
		HRESULT hr;
		IDirect3DTexture9* dxObj = ( o ) ? o->mDxObject : 0;
		hr = mDevice->SetTexture( 0, dxObj );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetTexture : INVALID CALL" );
		if ( mCurrentTexture ){
			mCurrentTexture->release(); //カウント減らし
			if ( mCurrentTexture->referenceCount() == 0 ){
				SAFE_DELETE( mCurrentTexture );
			}
		}
		mCurrentTexture = o;
		if ( o ){
			o->refer();
		}
	}
	void setVertexBuffer( VertexBuffer::Impl* o ){
		if ( mCurrentVertexBuffer == o ){
			return;
		}
		HRESULT hr;
		IDirect3DVertexBuffer9* dxObj = ( o ) ? o->mDxObject : 0;
		hr = mDevice->SetStreamSource( 0, dxObj, 0, sizeof( Vertex ) ); //Strideきめうち
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetStreamSource : INVALID CALL" );
		if ( mCurrentVertexBuffer ){
			mCurrentVertexBuffer->release();
			if ( mCurrentVertexBuffer->referenceCount() == 0 ){
				SAFE_DELETE( mCurrentVertexBuffer );
			}
		}
		mCurrentVertexBuffer = o;
		if ( o ){
			o->refer();
		}
	}
	void setIndexBuffer( IndexBuffer::Impl* o ){
		if ( mCurrentIndexBuffer == o ){
			return;
		}
		HRESULT hr;
		IDirect3DIndexBuffer9* dxObj = ( o ) ? o->mDxObject : 0;
		hr = mDevice->SetIndices( dxObj ); 
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetIndices : INVALID CALL" );
		if ( mCurrentIndexBuffer ){
			mCurrentIndexBuffer->release();
			if ( mCurrentIndexBuffer->referenceCount() == 0 ){
				SAFE_DELETE( mCurrentIndexBuffer );
			}
		}
		mCurrentIndexBuffer = o;
		if ( o ){
			o->refer();
		}
	}
	void enableDepthTest( bool f ){
		BOOL tf = ( f ) ? TRUE : FALSE;
		if ( mCurrentDepthTest == tf ){
			return;
		}
		HRESULT hr;
		hr = mDevice->SetRenderState( D3DRS_ZENABLE, tf );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState error : INVALID CALL" );
		mCurrentDepthTest = tf;
	}
	void enableDepthWrite( bool f ){
		BOOL tf = ( f ) ? TRUE : FALSE;
		if ( mCurrentDepthWrite == tf ){
			return;
		}
		HRESULT hr;
		hr = mDevice->SetRenderState( D3DRS_ZWRITEENABLE, tf );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState error : INVALID CALL" );
		mCurrentDepthWrite = tf;
	}
	void setBlendMode( BlendMode b ){
		BOOL alphaBlend;
		D3DBLEND dstBlend;
		BOOL alphaTest;
		if ( b == BLEND_OPAQUE ){
			alphaBlend = FALSE;
			dstBlend = D3DBLEND_INVSRCALPHA;
			alphaTest = TRUE;
		}else{
			alphaBlend = TRUE;
			alphaTest = FALSE;
			if ( b == BLEND_LINEAR ){
				dstBlend = D3DBLEND_INVSRCALPHA;
			}else{
				dstBlend = D3DBLEND_ONE;
			}
		}
		HRESULT hr;
		if ( mCurrentAlphaBlend != alphaBlend ){
			hr = mDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, alphaBlend );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );
			mCurrentAlphaBlend = alphaBlend;
		}
		if ( mCurrentDestBlend != dstBlend ){
			hr = mDevice->SetRenderState( D3DRS_DESTBLEND, dstBlend );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );
			mCurrentDestBlend = dstBlend;
		}
		if ( mCurrentAlphaTest != alphaTest ){
			hr = mDevice->SetRenderState( D3DRS_ALPHATESTENABLE, alphaTest );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );
			mCurrentAlphaTest = alphaTest;
		}
	}
	void setTextureFilter( TextureFilter f ){
		D3DTEXTUREFILTERTYPE dxMinF = D3DTEXF_NONE;
		D3DTEXTUREFILTERTYPE dxMagF = D3DTEXF_NONE;
		D3DTEXTUREFILTERTYPE dxMipF = D3DTEXF_NONE;
		if ( f == TEXTURE_FILTER_POINT ){
			dxMinF = D3DTEXF_POINT;
			dxMagF = D3DTEXF_POINT;
			dxMipF = D3DTEXF_NONE;
		}else if ( f == TEXTURE_FILTER_LINEAR ){ //リニアーとはいいつつアンイソGO!
			dxMinF = mBestMinFilter;
			dxMagF = mBestMagFilter;
			dxMipF = mBestMipFilter;
		}
		HRESULT hr;
		if ( mCurrentMinFilter != dxMinF ){
			mCurrentMinFilter = dxMinF;
			hr = mDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dxMinF );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetSamplerState : INVALID CALL" );
		}
		if ( mCurrentMagFilter != dxMagF ){
			mCurrentMagFilter = dxMagF;
			hr = mDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, dxMagF );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetSamplerState : INVALID CALL" );
		}
		if ( mCurrentMipFilter != dxMipF ){
			mCurrentMipFilter = dxMipF;
			hr = mDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, dxMipF );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetSamplerState : INVALID CALL" );
		}
	}
	void setCullMode( CullMode c ){
		D3DCULL mode = D3DCULL_NONE;
		switch ( c ){
			case CULL_NONE: mode = D3DCULL_NONE; break;
			case CULL_BACK: mode = D3DCULL_CW; break;
			case CULL_FRONT: mode = D3DCULL_CCW; break;
		}
		if ( mCurrentCullMode == mode ){
			return;
		}
		HRESULT hr;
		hr = mDevice->SetRenderState( D3DRS_CULLMODE, mode );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetRenderState : INVALID CALL" );

		mCurrentCullMode = mode;
	}
	void setProjectionViewMatrix( const Matrix44& m ){
		if ( mProjectionViewMatrix != m ){ //ちょっとでも違うなら
			mProjectionViewMatrix = m;
			mMatricesChanged = true;
		}
	}
	void setWorldMatrix( const Matrix34& m ){
		if ( mWorldMatrix != m ){ //ちょっとでも違うなら
			mWorldMatrix = m;
			mMatricesChanged = true;
		}
	}
	void sendMatrices(){
		HRESULT hr;
		//DX用0.5ピクセルずらし付き最終変換行列
		/*
		x,yをずらす行列Oをつくり
		O*PV*W
		と乗じて最終的な行列とする。

		が、Oは03,13の2要素に意味があり、他は意味がない行列だ。
		したがって、行列演算をフルセットやるのは馬鹿馬鹿しい。

		1 0 0 X    a b c d   a+Xm b+Xn c+Xo d+Xp
		0 1 0 Y  * e f g h = e+Ym f+Yn g+Yo h+Yp
		0 0 1 0    i j k l   i    j    k    l
		0 0 0 1    m n o p   m    n    o    p
		*/
		Matrix44 pvwm;
		pvwm.setMul( mProjectionViewMatrix, mWorldMatrix );
		float x = -1.f / static_cast< float >( mPresentParameters.BackBufferWidth );
		float y= 1.f / static_cast< float >( mPresentParameters.BackBufferHeight );
		pvwm.m00 += x * pvwm.m30;
		pvwm.m01 += x * pvwm.m31;
		pvwm.m02 += x * pvwm.m32;
		pvwm.m03 += x * pvwm.m33;
		pvwm.m10 += y * pvwm.m30;
		pvwm.m11 += y * pvwm.m31;
		pvwm.m12 += y * pvwm.m32;
		pvwm.m13 += y * pvwm.m33;
		//法線変換用ワールド逆行列転置
		Matrix34 itwm; //inverseTransposedWorldMatrix
		itwm.setInverse( mWorldMatrix );
		itwm.transpose33();

		//転送
		hr = mDevice->SetVertexShaderConstantF( 0, &( pvwm.m00 ), 4 );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetVertexShaderConstantF : INVALID CALL" );
		hr = mDevice->SetVertexShaderConstantF( 4, &( mWorldMatrix.m00 ), 3 );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetVertexShaderConstantF : INVALID CALL" );
		hr = mDevice->SetVertexShaderConstantF( 7, &( itwm.m00 ), 3 );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetVertexShaderConstantF : INVALID CALL" );

		mMatricesChanged = false;
	}
	void draw( int offset, int primitiveNumber, PrimitiveType prim = PRIMITIVE_TRIANGLE ){
		if ( mLightChanged ){
			sendLightingParameters();
		}
		if ( mMatricesChanged ){
			sendMatrices();
		}
		STRONG_ASSERT( mCurrentVertexBuffer && "VertexBuffer is not set." );
		if ( !mCurrentTexture ){ //テクスチャがないならダミーを差す。
			setTexture( mWhiteTexture );
		}
		D3DPRIMITIVETYPE dxPrim = D3DPT_TRIANGLELIST;
		switch ( prim ){
			case PRIMITIVE_TRIANGLE: dxPrim = D3DPT_TRIANGLELIST; break;
			case PRIMITIVE_LINE: dxPrim = D3DPT_LINELIST; break;
			case PRIMITIVE_POINT: dxPrim = D3DPT_POINTLIST; break;
			default: STRONG_ASSERT( 0 ); break;
		}
		HRESULT hr;
		hr = mDevice->DrawPrimitive( 
			dxPrim,
			offset,
			primitiveNumber );
		STRONG_ASSERT( SUCCEEDED( hr ) && "DrawPrimitive : INVALID CALL" );
	}
	void drawIndexed( int offset, int primitiveNumber, PrimitiveType prim = PRIMITIVE_TRIANGLE ){
		if ( mLightChanged ){
			sendLightingParameters();
		}
		if ( mMatricesChanged ){
			sendMatrices();
		}
		STRONG_ASSERT( mCurrentIndexBuffer && "IndexBuffer is not set." );
		STRONG_ASSERT( mCurrentVertexBuffer && "VertexBuffer is not set." );
		if ( !mCurrentTexture ){ //テクスチャがないならダミーを差す。
			setTexture( mWhiteTexture );
		}
		int vertexNumber = mCurrentVertexBuffer->mVertexNumber;
		D3DPRIMITIVETYPE dxPrim = D3DPT_TRIANGLELIST;
		switch ( prim ){
			case PRIMITIVE_TRIANGLE: dxPrim = D3DPT_TRIANGLELIST; break;
			case PRIMITIVE_LINE: dxPrim = D3DPT_LINELIST; break;
			case PRIMITIVE_POINT: dxPrim = D3DPT_POINTLIST; break;
			default: STRONG_ASSERT( 0 ); break;
		}
		HRESULT hr;
		hr = mDevice->DrawIndexedPrimitive( 
			dxPrim,
			0, 
			0,
			vertexNumber,
			offset,
			primitiveNumber );
		STRONG_ASSERT( SUCCEEDED( hr ) && "DrawPrimitive : INVALID CALL" );
	}
	void setViewport( int x, int y, int w, int h ){
		mViewport.X = static_cast< DWORD >( x );
		mViewport.Y = static_cast< DWORD >( y );
		mViewport.Width = static_cast< DWORD >( w );
		mViewport.Height = static_cast< DWORD >( h );
		//さて、本物のビューポートを作ろうか。
		float dw = static_cast< float >( mPresentParameters.BackBufferWidth );
		float dh = static_cast< float >( mPresentParameters.BackBufferHeight );
		float rw = static_cast< float >( mWidth );
		float rh = static_cast< float >( mHeight );
		float wRatio = dw / rw;
		float hRatio = dh / rh;
		float newX, newY, newW, newH;
		if ( wRatio > hRatio ){ //縦にあわせる。横が余る。
			newX = ( dw - hRatio * rw ) * 0.5f + static_cast< float >( x ) * hRatio;
			newY = 0.f;
			newW = static_cast< float >( w ) * hRatio;
			newH = dh;
		}else{ //横にあわせる
			newX = 0.f;
			newY = ( dh - wRatio * rh ) * 0.5f + static_cast< float >( y ) * wRatio;
			newW = dw;
			newH = static_cast< float >( h ) * wRatio;
		}
		D3DVIEWPORT9 vp;
		vp.MaxZ = mViewport.MaxZ;
		vp.MinZ = mViewport.MinZ;
		vp.Width = static_cast< DWORD >( newW + 0.5f );
		vp.Height = static_cast< DWORD >( newH + 0.5f );
		vp.X = static_cast< DWORD >( newX );
		vp.Y = static_cast< DWORD >( newY );
		HRESULT hr = mDevice->SetViewport( &vp );
		STRONG_ASSERT( hr != D3DERR_INVALIDCALL && "SetViewport : INVALID CALL" );
	}
	void getViewport( int* x, int* y, int* w, int* h ){
		if ( x ){
			*x = mViewport.X;
		}
		if ( y ){
			*y = mViewport.Y;
		}
		if ( w ){
			*w = mViewport.Width;
		}
		if ( h ){
			*h = mViewport.Height;
		}
	}
	void blendToScreen( Texture::Impl* t ){
		setTexture( t );
		setBlendMode( BLEND_LINEAR );
		enableDepthTest( false );
		enableDepthWrite( false );
		Matrix44 pvm;
		pvm.setIdentity();
		setProjectionViewMatrix( pvm );
		Matrix34 wm;
		wm.setIdentity();
		setWorldMatrix( wm );
		setVertexBuffer( mFullScreenQuadVertexBuffer );
		draw( 0, 1 );
		setTexture( 0 ); //後始末。setした人が戻す約束でやる。
	}
	void enableFullScreen( bool f ){
		if ( f != mFullScreen ){
			mFullScreen = f;
			mCanRender = false; //意図的にデバイスロスト状態に
		}
	}
	void getPointerModifier( float* scale, Vector2* offset ){
		float dw = static_cast< float >( mPresentParameters.BackBufferWidth );
		float dh = static_cast< float >( mPresentParameters.BackBufferHeight );
		float rw = static_cast< float >( mWidth );
		float rh = static_cast< float >( mHeight );
		float wRatio = rw / dw;
		float hRatio = rh / dh;
		if ( wRatio > hRatio ){ //横にあわせる。
			*scale = wRatio;
			offset->x = 0.f;
			offset->y = ( dh - ( rh / wRatio ) ) * -0.5f;
		}else{ //縦にあわせる
			*scale = hRatio;
			offset->x = ( dw - ( rw / hRatio ) ) * -0.5f;
			offset->y = 0.f;
		}
	}
	void setLightPosition( int index, const Vector3& position ){
		mLightPositionXs[ index ] = position.x;
		mLightPositionYs[ index ] = position.y;
		mLightPositionZs[ index ] = position.z;
		mLightChanged = true;
	}
	void setLightIntensity( int index, float intensity ){
		mLightIntensities[ index ] = intensity;
		mLightChanged = true;
	}
	void setLightColor( int index, const Vector3& color ){
		mLightColors[ index ] = color;
		mLightChanged = true;
	}
	void setEyePosition( const Vector3& position ){
		mEyePosition = position;
		mLightChanged = true;
	}
	void setDiffuseColor( const Vector3& c ){
		mDiffuseColor.x = c.x;
		mDiffuseColor.y = c.y;
		mDiffuseColor.z = c.z;
		mLightChanged = true;
	}
	void setSpecularColor( const Vector3& c ){
		mSpecularColor.x = c.x;
		mSpecularColor.y = c.y;
		mSpecularColor.z = c.z;
		mLightChanged = true;
	}
	void setEmissionColor( const Vector3& c ){
		mEmissionColor.x = c.x;
		mEmissionColor.y = c.y;
		mEmissionColor.z = c.z;
		mLightChanged = true;
	}
	void setAmbientColor( const Vector3& c ){
		mAmbientColor.x = c.x;
		mAmbientColor.y = c.y;
		mAmbientColor.z = c.z;
		mLightChanged = true;
	}
	void setTransparency( float t ){
		mDiffuseColor.w = t;
		mLightChanged = true;
	}
	void setSpecularSharpness( float e ){
		mSpecularColor.w = e;
		mLightChanged = true;
	}
	void sendLightingParameters(){
		//ライト色は毎回強度と色を掛け合わせる。
		Vector4 lc[ 4 ];
		for ( int i = 0; i < 4; ++i ){
			lc[ i ].x = mLightColors[ i ].x * mLightIntensities[ i ];
			lc[ i ].y = mLightColors[ i ].y * mLightIntensities[ i ];
			lc[ i ].z = mLightColors[ i ].z * mLightIntensities[ i ];
			lc[ i ].w = 0.f;
		}
		//ディフューズカラーはいつでも送る
		HRESULT hr;
		hr = mDevice->SetVertexShaderConstantF( 10, &mDiffuseColor.x, 1 );
		STRONG_ASSERT( SUCCEEDED( hr ) && "SetVertexShaderConstantF : INVALID CALL" );
		if ( mLightingMode == LIGHTING_PER_PIXEL ){
			hr = mDevice->SetPixelShaderConstantF( 0, &mAmbientColor.x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetPixelShaderConstantF : INVALID CALL" );
			hr = mDevice->SetPixelShaderConstantF( 1, &mLightPositionXs.x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetPixelShaderConstantF : INVALID CALL" );
			hr = mDevice->SetPixelShaderConstantF( 2, &mLightPositionYs.x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetPixelShaderConstantF : INVALID CALL" );
			hr = mDevice->SetPixelShaderConstantF( 3, &mLightPositionZs.x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetPixelShaderConstantF : INVALID CALL" );
			hr = mDevice->SetPixelShaderConstantF( 4, &mEyePosition.x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetPixelShaderConstantF : INVALID CALL" );
			//スペキュラ色は強度を調整2+e/2pi
			Vector4 sc;
			const float rcpPi2 = 1.f / ( 3.1415926535897932384626433832795f * 2.f );
			float f = ( 2.f + mSpecularColor.w ) * rcpPi2;
			sc.x = mSpecularColor.x * f;
			sc.y = mSpecularColor.y * f;
			sc.z = mSpecularColor.z * f;
			sc.w = mSpecularColor.w;

			hr = mDevice->SetPixelShaderConstantF( 5, &sc.x, 1 );
//			hr = mDevice->SetPixelShaderConstantF( 5, &mSpecularColor.x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetPixelShaderConstantF : INVALID CALL" );
			hr = mDevice->SetPixelShaderConstantF( 6, &lc[ 0 ].x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetPixelShaderConstantF : INVALID CALL" );
			hr = mDevice->SetPixelShaderConstantF( 7, &lc[ 1 ].x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetPixelShaderConstantF : INVALID CALL" );
			hr = mDevice->SetPixelShaderConstantF( 8, &lc[ 2 ].x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetPixelShaderConstantF : INVALID CALL" );
			hr = mDevice->SetPixelShaderConstantF( 9, &lc[ 3 ].x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetPixelShaderConstantF : INVALID CALL" );
			hr = mDevice->SetPixelShaderConstantF( 10, &mEmissionColor.x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetPixelShaderConstantF : INVALID CALL" );
		}else if ( mLightingMode == LIGHTING_PER_VERTEX ){
			hr = mDevice->SetVertexShaderConstantF( 11, &mAmbientColor.x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetVertexShaderConstantF : INVALID CALL" );
			hr = mDevice->SetVertexShaderConstantF( 12, &mLightPositionXs.x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetVertexShaderConstantF : INVALID CALL" );
			hr = mDevice->SetVertexShaderConstantF( 13, &mLightPositionYs.x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetVertexShaderConstantF : INVALID CALL" );
			hr = mDevice->SetVertexShaderConstantF( 14, &mLightPositionZs.x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetVertexShaderConstantF : INVALID CALL" );
			hr = mDevice->SetVertexShaderConstantF( 15, &lc[ 0 ].x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetVertexShaderConstantF : INVALID CALL" );
			hr = mDevice->SetVertexShaderConstantF( 16, &lc[ 1 ].x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetVertexShaderConstantF : INVALID CALL" );
			hr = mDevice->SetVertexShaderConstantF( 17, &lc[ 2 ].x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetVertexShaderConstantF : INVALID CALL" );
			hr = mDevice->SetVertexShaderConstantF( 18, &lc[ 3 ].x, 1 );
			STRONG_ASSERT( SUCCEEDED( hr ) && "SetVertexShaderConstantF : INVALID CALL" );
		}
		mLightChanged = false;
	}
	void setLightingMode( LightingMode lm ){
		if ( !mPixelShaderReady && ( lm == LIGHTING_PER_PIXEL ) ){
			cout << "Graphics::enablePixelShader() : This hardware doesn't support pixel shader( > 2.0 ). do per vertex." << endl;
			lm = LIGHTING_PER_VERTEX;
		}
		if ( lm != mLightingMode ){
			mLightChanged = true;
			mLightingMode = lm;
			setShader();
		}
	}
	IDirect3DDevice9* dxDevice(){
		return mDevice;
	}
	IDirect3D9* mDirect3d;
	IDirect3DDevice9* mDevice;
	D3DVIEWPORT9 mViewport;
	D3DPRESENT_PARAMETERS mPresentParameters;
	int mWidth;
	int mHeight;
	bool mFullScreen;
	bool mVSync;
	bool mPixelShaderReady;
	LightingMode mLightingMode;
	int mFullScreenWindowWidth;
	int mFullScreenWindowHeight;
	unsigned mFrameId;
	bool mCanRender;
	HWND mWindowHandle;

	IDirect3DVertexDeclaration9* mVertexDeclaration;
	IDirect3DVertexShader9* mNoLightingVertexShader;
	IDirect3DVertexShader9* mVertexLightingVertexShader;
	IDirect3DVertexShader9* mPixelLightingVertexShader;
	IDirect3DPixelShader9* mPixelLightingPixelShader;

	VertexBuffer::Impl* mFullScreenQuadVertexBuffer;
	Texture::Impl* mWhiteTexture;

	//状態変数群
	Texture::Impl* mCurrentTexture;
	VertexBuffer::Impl* mCurrentVertexBuffer;
	IndexBuffer::Impl* mCurrentIndexBuffer;
	BOOL mCurrentDepthTest;
	BOOL mCurrentDepthWrite;
	D3DCULL mCurrentCullMode;
	BOOL mCurrentAlphaTest;
	BOOL mCurrentAlphaBlend;
	D3DBLEND mCurrentDestBlend;
	D3DTEXTUREFILTERTYPE mCurrentMinFilter;
	D3DTEXTUREFILTERTYPE mCurrentMagFilter;
	D3DTEXTUREFILTERTYPE mCurrentMipFilter;
	D3DTEXTUREFILTERTYPE mBestMinFilter;
	D3DTEXTUREFILTERTYPE mBestMagFilter;
	D3DTEXTUREFILTERTYPE mBestMipFilter;
	DWORD mMaxAnisotropy;
	DWORD mMsaaQualityFullScreen;
	DWORD mMsaaQualityWindowed;

	Vector4 mLightPositionXs;
	Vector4 mLightPositionYs;
	Vector4 mLightPositionZs;
	Vector3 mLightColors[ 4 ];
	float mLightIntensities[ 4 ];
	Vector4 mEyePosition;
	Vector4 mDiffuseColor;
	Vector4 mSpecularColor;
	Vector4 mEmissionColor;
	Vector4 mAmbientColor;
	bool mLightChanged;

	Matrix44 mProjectionViewMatrix;
	Matrix34 mWorldMatrix;
	bool mMatricesChanged;
	bool mCaptureRequest;
	string mCaptureFilename;
};
extern ManagerImpl* gManagerImpl; //唯一インスタンス

} //namespace Graphics
} //namespace GameLib

#endif
