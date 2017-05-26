#ifndef INCLUDED_GAMELIB_SCENE_FONT_H
#define INCLUDED_GAMELIB_SCENE_FONT_H

namespace GameLib{

namespace Graphics{
	class Texture;
}
namespace Math{
	class Vector2;
}
using namespace Math;

namespace Scene{

///フォントクラス
class Font{
public:
	///ビットマップフォントの文字一つづつに付き必要なデータ
	struct BitmapChar{
		unsigned short mCode; //文字コード(SHIFT_JIS)
		unsigned short mX; //左端
		unsigned short mY; //上端
		unsigned char mTexturePage; //テクスチャページ
		unsigned char mWidth; //幅情報(ピクセル)
		unsigned char mHeight; //高さ情報(ピクセル)
	};
	///テクスチャを使ったビットマップフォントを生成。文字の高さは最大値を採用。
	static Font create( 
		const char** textureFileNames, //テクスチャファイル名配列
		int textureNumber,
		BitmapChar* BitmapChars,
		int bitmapCharNumber );
	///テクスチャを使ったビットマップフォントを生成。文字の高さは最大値を採用。
	static Font create(
		const char** textureFileData, //テクスチャDDSイメージ配列
		const int* textureFileSizes, //テクスチャデータサイズ配列
		int textureNumber,
		BitmapChar* bitmapChars,
		int CharNumber );
	///OSから動的にフォントデータを取得する。文字の高さ、内部テクスチャ幅などは指定。
	static Font create(
		const char* osFontName,
		int charHeightHint, //これは目安。実際にいくつかはgetCharHeightを参照せよ
		bool bold = false,
		bool italic = false,
		bool proportional = false,
		bool antialias = false,
		int internalTextureWidth = 512,
		int internalTextureHeight = 512 );
	///文字情報を取得。見つからないとfalseを返す。
	bool getChar( 
		Graphics::Texture* textureOut,
		Vector2* uvTopLeftOut,
		Vector2* uvBottomRightOut,
		int code );
	///文字の最大高さ
	int charHeight() const;
	///ロード終わってる？
	bool isReady();

	//以下ユーザは意識しなくていい
	Font();
	~Font();
	Font( const Font& );
	operator void*() const;
	Font& operator=( const Font& );
	void release();
	bool operator==( const Font& ) const;
	bool operator!=( const Font& ) const;

	class Impl;
private:
	explicit Font( Impl* );
	Impl* mImpl;
};

} //namespace Scene
} //namespace GameLib

#endif
