#include "GameLib/GameLib.h"
#include "PseudoXml/Tag.h"
#include "PseudoXml/AttributeImpl.h"
#include "PseudoXml/DocumentImpl.h"
#include "GameLib/Base/RefString.h"
using namespace std;

namespace GameLib{
namespace PseudoXml{

//状態遷移表。
/*
<状態番号リスト>
0:<の上。初期状態
1:エレメント名文字列
2:エレメント名の後の空白。アトリビュートの間の空白。
3:アトリビュート名
4:=の上
5:アトリビュート値(""の中身)
6:空要素タグ記号/の上
7:アトリビュート名と=の間の空白
8:アトリビュート値(''の中身)
9:アトリビュート値(くるまれてない奴)
10:コメント <! ... -->がコメント。本当は<!-- ... -->だが、!が出た段階で普通じゃないし、無視。(実はまずいんだがな...<!ELEMENTみたいなのもある)
11:コメント中-発見
12:コメント中--発見
13:?コメント中 <? ... ?>がコメント
14:?コメント中?発見
E:終了

<状態遷移リスト>
記号:
c = [a-zA-Z0-9_] つまり普通の文字
s = 他の条件に引っかからない残りの文字

0,/,0 終了タグフラグ立てる
0,!,10 コメントじゃん
0,?,13 コメントじゃん
0,c,1 エレメント名文字列最初の一文字
0,s,0 とどまる
1,c,1 エレメント名文字列に追加
1,>,E
1,s,2 エレメント名確定
2,>,E
2,/,6 空要素タグ/発見
2,c,3 アトリビュート名最初の一文字
2,s,2 読み飛ばす
3,=,4 =発見。アトリビュート名確定
3,c,3 アトリビュート名に追加
3,s,7 空白へ。アトリビュート名確定
4,",5 ダブルクォテイション発見。アトリビュート値に移行
4,/,8 シングルクォテイション発見。アトリビュート値に移行
4,c,9 アトリビュート値に移行
4,s,4 読み飛ばす
5,",2 アトリビュート値確定。アトリビュートをリストに追加。
5,s,5 アトリビュート値文字列に追加
6,>,E 空要素として終わる
6,s,6 読み飛ばす
7,=,4 =発見
7,s,7 読み飛ばす
8,',2 アトリビュート値確定。
8,s,8 アトリビュート値に追加
9,/,6 空要素タグ発見。
9,>,E
9,c,9 アトリビュート値に追加
9,s,2 アトリビュート値確定
10,-,11
10,s,10
11,-,12
11,s,10
12,>,E
12,s,10
13,?,14
13,s,13
14,>,E
14,s,13
*/
Tag::Tag( const char** p, const char* e, Document::Impl* document ) : 
mType( TYPE_BEGIN ),
mDocument( document ){
	//エレメント名、アトリビュートの名前と値の一時的な格納場所
	const char* eName = 0;
	int eNameSize = 0;
	const char* aName = 0;
	int aNameSize = 0;
	const char* aValue = 0;
	int aValueSize = 0;

	int m = 0; //モード。modeとかにしたほうがいいんだろうが、使用回数が尋常じゃないので1文字にした
	bool end = false; //ループを抜けるフラグ
	while ( *p < e ){
		bool wide = false; //日本語ですか？
		char c = **p;
		switch ( m ){
			case 0: //初期状態
				if ( c == '/' ){
					mType = TYPE_END;
				}else if ( c == '!' ){ //コメント
					mType = TYPE_COMMENT;
					m = 10;
				}else if ( c == '?'){
					mType = TYPE_COMMENT;
					m = 13;
				}else if ( isNormalChar( c, &wide ) ){
					eName = *p;
					eNameSize = 1;
					if ( wide && ( ( *p + 1 ) < e ) ){
						++eNameSize;
					}
					m = 1;
				}else{
					; //何もしない
				}
				break;
			case 1: //エレメント名
				if ( c == '>' ){
					end = true;
				}else if ( isNormalChar( c, &wide ) ){
					++eNameSize;
					if ( wide && ( ( *p + 1 ) < e ) ){
						++eNameSize;
					}
				}else{
					m = 2; //エレメント名を抜ける
				}
				break;
			case 2: //エレメント名の後の空白
				if ( c == '>' ){
					end = true;
				}else if ( c == '/' ){
					mType = TYPE_BEGIN_END; //空要素タグです
					m = 6;
				}else if ( isNormalChar( c, &wide ) ){
					aName = *p;
					aNameSize = 1;
					if ( wide && ( ( *p + 1 ) < e ) ){
						++aNameSize;
					}
					m = 3;
				}else{
					; //何もしない
				}
				break;
			case 3: //アトリビュート名
				if ( c == '=' ){
					m = 4;
				}else if ( isNormalChar( c, &wide ) ){
					++aNameSize;
					if ( wide && ( ( *p + 1 ) < e ) ){
						++aNameSize;
					}
				}else{
					m = 7; //空白へ
				}
				break;
			case 4: //=の上
				if ( c == '"' ){
					aValue = *p + 1;
					m = 5;
				}else if ( c == '\'' ){
					aValue = *p + 1;
					m = 8;
				}else if ( isNormalChar( c, &wide ) ){
					aValue = *p;
					aValueSize = 1;
					if ( wide && ( ( *p + 1 ) < e ) ){
						++aValueSize;
					}
					m = 9;
				}else{
					; //何もしない
				}
				break;
			case 5: //アトリビュート値
				if ( c == '"' ){
					m = 2; //エレメント名の後の空白
					addAttribute( aName, aNameSize, aValue, aValueSize );
					aNameSize = aValueSize = 0;
				}else{
					++aValueSize;
					isNormalChar( c, &wide ); //ワイドかどうかだけ知りたい
					if ( wide && ( ( *p + 1 ) < e ) ){
						++aValueSize;
					}
				}
				break;
			case 6: //空要素タグの/出現後
				switch ( c ){
					case '>': end = true; break;
					default: break; //読み飛ばし続ける
				}
				break;
			case 7: //アトリビュート名の後の空白
				if ( c == '=' ){
					m = 4;
				}else if ( c == '>' ){
					addAttribute( aName, aNameSize, aValue, aValueSize );
					aNameSize = aValueSize = 0;
					end = true;
				}else if ( isNormalChar( c, &wide ) ){ //値のないアトリビュート
					addAttribute( aName, aNameSize, aValue, aValueSize );
					aNameSize = aValueSize = 0;
					//次のアトリビュートに入れます
					aName = *p;
					aNameSize = 1;
					if ( wide && ( ( *p + 1 ) < e ) ){
						++aNameSize;
					}
					m = 3;
				}else{
					; //何もしない
				}
				break;
			case 8: //アトリビュート値
				if ( c == '\'' ){
					m = 2; //エレメント名の後の空白
					addAttribute( aName, aNameSize, aValue, aValueSize );
					aNameSize = aValueSize = 0;
				}else{
					++aValueSize;
					isNormalChar( c, &wide ); //ワイドかどうかだけ知りたい
					if ( wide && ( ( *p + 1 ) < e ) ){
						++aValueSize;
					}
				}
				break;
			case 9: //アトリビュート値
				if ( c == '/' ){
					mType = TYPE_BEGIN_END; //空要素タグです
					m = 6;
					addAttribute( aName, aNameSize, aValue, aValueSize );
					aNameSize = aValueSize = 0;
				}else if ( c == '>' ){
					addAttribute( aName, aNameSize, aValue, aValueSize );
					aNameSize = aValueSize = 0;
					end = true;
				}else if ( isNormalChar( c, &wide ) ){
					++aValueSize;
					if ( wide && ( ( *p + 1 ) < e ) ){
						++aValueSize;
					}
				}else{
					m = 2; //エレメント名の後の空白
					addAttribute( aName, aNameSize, aValue, aValueSize );
					aNameSize = aValueSize = 0;
				}
			case 10: //コメント
				switch ( c ){
					case '-': m = 11; break;
					default: break; //読み飛ばし続ける
				}
				break;
			case 11: //コメント-
				switch ( c ){
					case '-': m = 12; break;
					default: m = 10; break; //コメント続く
				}
				break;
			case 12: //コメント--
				switch ( c ){
					case '>': end = true; break;
					default: m = 10; break; //コメント続く
				}
				break;
			case 13: //?コメント
				switch ( c ){
					case '?': m = 14; break;
					default: break; //読み飛ばし続ける
				}
				break;
			case 14: //?コメント?
				switch ( c ){
					case '>': end = true; break;
					default: m = 13; break; //コメント続く
				}
				break;
		}
		*p += ( wide ) ? 2 : 1; //進める
		if ( end ){ //終わりです 進めてから終わることに注意しよう。上の行と逆になるとバグる
			break;
		}
	}
	mName.set( eName, eNameSize );
}

} //namespace PseudoXml
} //namespace GameLib
