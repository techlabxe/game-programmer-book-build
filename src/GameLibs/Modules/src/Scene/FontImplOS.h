#ifndef INCLUDED_GAMELIB_SCENE_FONTIMPLOS_H
#define INCLUDED_GAMELIB_SCENE_FONTIMPLOS_H

#include "Scene/FontImpl.h"
#include "GameLib/Graphics/Texture.h"
#include "GameLib/Graphics/Manager.h"
#include "GameLib/Math/Vector2.h"
#include "GameLib/WindowCreator/WindowCreator.h"
#include <map> //いずれ自作に換えたい...

#include <windows.h>
#undef min
#undef max

namespace GameLib{
namespace Scene{
using namespace std;

using namespace GameLib::Graphics;
using namespace GameLib::Math;

namespace { //無名

//Windowsからフォントデータを取得
class FontImplOS : public Font::Impl{
	struct Char;
public:
	FontImplOS(
	const char* fontName,
	int charHeightHint, //これになるとは限らない
	bool bold,
	bool italic,
	bool proportional,
	bool antialias,
	int textureWidth,
	int textureHeight ) :
	mCharHeight( 0 ),
	mCharAscent( 0 ),
	mAntiAliased( antialias ),
	mFontHandle( 0 ),
	mDeviceContextHandle( 0 ),
	mCurrentX( 0 ),
	mCurrentY( 0 ),
	mCurrentLineHeight( 0 ),
	mCurrentPage( 0 ),
	mTextureWidth( textureWidth ),
	mTextureHeight( textureHeight ){
		//テクスチャ生成。とりあえず一枚
		Texture t = Texture::create( mTextureWidth, mTextureHeight, false );
		//マップに格納
		mTextures.insert( make_pair( 0, t ) );

		//フォント名をUNICODE変換
		DWORD pitch = ( proportional ) ? VARIABLE_PITCH : FIXED_PITCH;

		//フォント生成
		mFontHandle = CreateFontA(
			charHeightHint,
			0,
			0,
			0,
			( bold ) ? FW_BOLD : FW_DONTCARE,
			( italic ) ? TRUE : FALSE,
			FALSE,
			FALSE,
			SHIFTJIS_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY,
			pitch | FF_DONTCARE,
			fontName );
		ASSERT( mFontHandle );
		//HDC生成
		mDeviceContextHandle = CreateCompatibleDC( NULL );
		//テキストメトリクス取得
		//fontセット
		SelectObject( mDeviceContextHandle, mFontHandle );
		//文字のメトリクス生成
		TEXTMETRIC tm;
		GetTextMetrics( mDeviceContextHandle, &tm );
		mCharHeight = tm.tmHeight; //実際の値で上書き。
		mCharAscent = tm.tmAscent; //ベースラインの上の寸法が必要
	}
	~FontImplOS(){
		DeleteDC( mDeviceContextHandle );
		mDeviceContextHandle = 0;
	
		DeleteObject( mFontHandle );
		mFontHandle = 0;
	}
	bool getChar(
	Texture* texture,
	Vector2* uvTopLeft,
	Vector2* uvBottomRight,
	int code ){
		//コードテーブルにあれば使う。
		CharIt cit = mChars.find( static_cast< unsigned short >( code ) );
		const Char* c = 0;
		if ( cit != mChars.end() ){
			c = &cit->second;
		}else{
			c = createGlyph( code );
		}
		TextureIt tit = mTextures.find( c->mTexturePage );
		ASSERT( tit != mTextures.end() );
		*texture = tit->second;
		float tw = static_cast< float >( texture->width() );
		float th = static_cast< float >( texture->height() );
		float w = static_cast< float >( c->mWidth );
		float h = static_cast< float >( c->mHeight );
		float x = static_cast< float >( c->mX );
		float y = static_cast< float >( c->mY );
		uvTopLeft->set( x / tw, y / th );
		uvBottomRight->set( 
			( x + w ) / tw, 
			( y + h ) / th );
		return true;
	}
	const Char* createGlyph( int code ){
		//fontセット
		SelectObject( mDeviceContextHandle, mFontHandle );

		GLYPHMETRICS gm; // 情報
		MAT2 mat = {
			{ 0, 1 }, 
			{ 0, 0 }, 
			{ 0, 0 }, 
			{ 0, 1 }
		};
		DWORD size;
		const UINT format = ( mAntiAliased ) ? GGO_GRAY8_BITMAP : GGO_BITMAP;
		// まずは必要なバッファサイズを取得
		size = GetGlyphOutlineA(
			mDeviceContextHandle,
			static_cast< UINT >( code ),
			format,
			&gm, 0, NULL, &mat );
		//寸法取得
		int cellW = static_cast< int >( gm.gmCellIncX );
		int cellH = static_cast< int >( mCharHeight ); //IncYは役立たず。
		int glyphW = static_cast< int >( gm.gmBlackBoxX );
		int glyphH = static_cast< int >( gm.gmBlackBoxY );
		int glyphX = static_cast< int >( gm.gmptGlyphOrigin.x );
		int glyphY = static_cast< int >( mCharAscent - gm.gmptGlyphOrigin.y );
		if ( glyphW + glyphX > cellW ){ //ありえんが対処するほかあるまい
			cellW = glyphW + glyphX;
		}
		if ( glyphH + glyphY > cellH ){ //ありえんが対処するほかあるまい
			cellH = glyphH + glyphY;
		}
		//書き込みテクスチャ取得
		TextureIt tit = mTextures.find( mCurrentPage );
		ASSERT( tit != mTextures.end() );
		Texture* tex = &( tit->second );
		int tw = tex->width();
		int th = tex->height();
		//横あふれました！
		if ( mCurrentX + cellW >= tw ){
			mCurrentX = 0;
			mCurrentY += mCurrentLineHeight;
			mCurrentLineHeight = 0;
			//縦あふれました！
			if ( mCurrentY + cellH >= th ){
				mCurrentY = 0;
				++mCurrentPage;
				Texture t = Texture::create( mTextureWidth, mTextureHeight, false );
				//マップに格納
				mTextures.insert( make_pair( mCurrentPage, t ) );
				//テクスチャ取り直し
				TextureIt tit = mTextures.find( mCurrentPage );
				ASSERT( tit != mTextures.end() );
				tex = &( tit->second );
			}
		}
		//テクスチャロック(部分ロック)
		unsigned* dst;
		int dstPitch;
		tex->lock( 
			&dst, 
			&dstPitch,
			mCurrentX,
			mCurrentY,
			cellW,
			cellH );

		if ( size == 0 ){ //サイズ分ただ0埋め
			for ( int y = 0; y < cellH; ++y ){
				for ( int x = 0; x < cellW; ++x ){
					dst[ x ] = 0x00ffffff;
				}
				dst += dstPitch / 4;
			}
		}else{
			// バッファを確保
			BYTE* data = NEW BYTE[ size ];
			int srcPitch = ( glyphW + 3 ) & ~( 3 ); //4バイトアライン

			// 二度目の呼び出しが本物。データ取得。
			size = GetGlyphOutlineA(
				mDeviceContextHandle,
				static_cast< UINT >( code ),
				format,
				&gm, size, data, &mat );
			ASSERT( size != GDI_ERROR );

			//1bitの時が不便すぎるので、バイト配列にコピー
			if ( mAntiAliased ){ //アンチがかかってるなら64->255階調変換
				for ( DWORD i = 0; i < size; ++i ){
					data[ i ] = data[ i ] * 255 / 64;
				}
			}else{
				BYTE* newData = NEW BYTE[ size * 8 ]; //8倍あれば足りるだろ
				const BYTE* src = data;
				BYTE* dst = newData;
				int srcPitch = ( ( ( glyphW + 7 ) / 8 ) + 3 ) & ( ~3 );
				int dstPitch = ( glyphW + 3 ) & ( ~3 );
				for ( int y = 0; y < glyphH; ++y ){
					for ( int x = 0; x < glyphW; ++x ){
						int shift = 7 - ( x % 8 );
						int mask = 0x1 << shift;
						int bit = ( src[ x / 8 ] & mask ) >> shift;
						dst[ x ] = static_cast< BYTE >( bit * 255 );
					}
					dst += dstPitch;
					src += srcPitch;
				}
				//旧配列をdelete l|
				SAFE_DELETE_ARRAY( data );
				//新配列に差し替え
				data = newData;
			}
			BYTE* src = data;
			//上部空白の塗りつぶし
			int y = 0;
			for ( ; y < glyphY; ++y ){
				for ( int x = 0; x < cellW; ++x ){
					dst[ x ] = 0x00ffffff;
				}
				dst += dstPitch / 4;
			}
			//グリフ本体Z
			for ( ; y < glyphY + glyphH; ++y ){
				LONG x = 0;
				//左側空白
				for ( ; x < glyphX; ++x ){
					dst[ x ] = 0x00ffffff;
				}
				//グリフ本体
				for ( ; x < glyphX + glyphW; ++x ){
					int a = src[ x - glyphX ];
					dst[ x ] = ( a << 24 ) | 0xffffff;
				}
				//右側空白
				for ( ; x < cellW; ++x ){
					dst[ x ] = 0x00ffffff;
				}
				dst += dstPitch / 4;
				src += srcPitch;
			}
			//下部空白の塗りつぶし
			for ( ; y < cellH; ++y ){
				for ( int x = 0; x < cellW; ++x ){
					dst[ x ] = 0x00ffffff;
				}
				dst += dstPitch / 4;
			}
			SAFE_DELETE_ARRAY( data );
		}
		tex->unlock( &dst, 0 );
		//リストに追加
		Char c;
		c.mTexturePage = static_cast< unsigned char >( mCurrentPage );
		c.mWidth = static_cast< unsigned char >( cellW );
		c.mHeight = static_cast< unsigned char >( cellH );
		c.mX = static_cast< unsigned short >( mCurrentX );
		c.mY = static_cast< unsigned short >( mCurrentY );
		mCurrentX += cellW; //書き込み位置進める
		if ( mCurrentLineHeight < cellH ){
			mCurrentLineHeight = cellH;
		}

		CharIt it = mChars.insert( make_pair( code, c ) ).first;
		return &( it->second );
	}
	int charHeight() const {
		return mCharHeight;
	}
private:
	void operator=( const FontImplOS& ); //禁止

	struct Char{
		unsigned char mTexturePage; //テクスチャページ
		unsigned char mHeight; //高さ情報(ピクセル)
		unsigned char mWidth; //幅情報(ピクセル)
		unsigned short mX; //左端
		unsigned short mY; //上端
	};
	typedef map< unsigned short, Char > CharMap;
	typedef CharMap::iterator CharIt;
	typedef map< int, Texture > TextureMap;
	typedef TextureMap::iterator TextureIt;

	int mCharHeight;
	int mCharAscent; //TEXTMETRIXから取得
	bool mAntiAliased;
	HFONT mFontHandle;
	HDC mDeviceContextHandle;
	CharMap mChars;
	TextureMap mTextures;
	int mCurrentX; //次の文字の左上X
	int mCurrentY; //次の文字の左上Y
	int mCurrentLineHeight; //現在の行の最大高さ
	int mCurrentPage; //現在書き込み中のテクスチャ番号
	const int mTextureWidth;
	const int mTextureHeight;
};

} //namespace {}

} //Scene:
} //GameLib
#endif
