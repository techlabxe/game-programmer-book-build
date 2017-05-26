#ifndef INCLUDED_STRING_RENDERER_H
#define INCLUDED_STRING_RENDERER_H

class Image;

//文字列描画クラス
//シングルトンパターンのサンプルでもある。
class StringRenderer{
public:
	static StringRenderer* instance();
	//描画関数。デフォルト白
	void draw( int x, int y, const char* string, unsigned color = 0xffffff ) const;
	static void create( const char* fontFileName ); //これを呼んで初期化
	static void destroy(); //これを呼んで終了
private:
	StringRenderer( const char* fontFileName ); //プライベートにする。
	StringRenderer( StringRenderer& ); //コピーコンストラクタ抹殺。cppに中身を作らない。
	~StringRenderer(); //プライベートにする。

	Image* mImage;
	static StringRenderer* mInstance; //唯一のインスタンス
};

#endif