#ifndef INCLUDED_GAMELIB_GRAPHICS_TEXTURE_H
#define INCLUDED_GAMELIB_GRAPHICS_TEXTURE_H

namespace GameLib{
namespace PseudoXml{
	class ConstElement;
}
namespace Graphics{

class Texture{
public:
	///エレメントから生成。pathがあればファイル名の前に追加する。
	static Texture create( PseudoXml::ConstElement, const char* path = 0 );
	//空のテクスチャ
	static Texture create( int width, int height, bool createMipChain );
	///TGAもしくはDDS。拡張子で判別する。
	static Texture create( const char* filename );
	///TGAもしくはDDSの中身を渡す。最初にDDSと書いてあればDDSとみなし、それ以外はTGAとみなす。
	static Texture create( const char* fileData, int fileSize );
	///名前ゲット(ニセxmlからロードすればその名前。他は空。)
	const char* name() const;

	///ロード終わった？(constがないのは内部で処理があるから)
	bool isReady(); 
	///読み込みエラー起こってない？
	bool isError() const;

	///書き込み関数
	void lock( unsigned** address, int* pitch, int mipLevel = 0 );
	///部分書き込み関数
	void lock( 
		unsigned** address, 
		int* pitch, 
		int x,
		int y,
		int width,
		int height,
		int mipLevel = 0 );
	void unlock( unsigned** address, int mipLevel = 0 );
	//情報取得
	int width() const;
	int height() const;
	///2冪に直される前の元の幅
	int originalWidth() const;
	///2冪に直される前の元の高さ
	int originalHeight() const;
	int mipmapNumber() const;
	///ファイルに書き出す。成功失敗はチェックしない。デバグ用。
	void write( const char* filename );

	//以下ユーザは意識しない関数群
	Texture();
	Texture( const Texture& );
	~Texture();
	operator void*() const;
	Texture& operator=( const Texture& );
	void release();
	bool operator==( const Texture& ) const;
	bool operator!=( const Texture& ) const;
	bool operator<( const Texture& ) const;

	class Impl;
	Texture( const Impl* );
private:
	friend class Manager;
	Impl* mImpl;
};

} //namespace Graphics
} //namespace GameLib

#endif
