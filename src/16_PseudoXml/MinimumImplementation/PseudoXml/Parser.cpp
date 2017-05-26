#include "GameLib/GameLib.h"
#include "PseudoXml/Document.h"
#include "PseudoXml/Element.h"
#include "PseudoXml/Attribute.h"
#include "PseudoXml/Parser.h"

namespace PseudoXml{

class ElementData{
public:
	~ElementData(){
		//空のはずです
		assert( mAttributes.size() == 0 && mChildren.size() == 0 );
	}
	string mName; //エレメント名
	list< Attribute* > mAttributes; //アトリビュート
	list< ElementData* > mChildren; //子供データ
};

Parser::Parser( const char* data, int size ) :
mData( data ),
mDataSize( size ),
mMode( 0 ),
mInEndTag( false ){
}

Parser::~Parser(){
	//ここに来るときには空になっているはず
	assert( mElements.size() == 0 );
}

namespace {
//a-z,A-Z,0-9,_のどれかかを調べる関数
//わかりやすいが遅い。高速化の方法を検討してみよう。
bool static isNormalChar( char c ){
	if ( c >= '0' && c <= '9' ){
		return true;
	}
	if ( c >= 'a' && c <= 'z' ){
		return true;
	}
	if ( c >= 'A' && c <= 'Z' ){
		return true;
	}
	if ( c == '_' ){
		return true;
	}
	return false;
}

} //namespace {}

//状態遷移表。最低限なので、エラーには異常なほど寛容な作りになっている。
/*
<状態番号リスト>
0:初期状態
1:<の上
2:エレメント名文字列
3:エレメント名の後の空白。アトリビュートの間の空白。
4:アトリビュート名
5:=の上
6:アトリビュート値(""の中身)
7:>の上

<状態遷移リスト>
記号:
c = [a-zA-Z0-9_] つまり普通の文字
s = 他の条件に引っかからない残りの文字

0,<,1 開始
1,/,1 終了タグフラグ立てる
1,s,2 エレメント名文字列最初の一文字
2,c,2 エレメント名文字列に追加
2,>,7
2,s,3 エレメント名確定
3,>,7
3,c,4 アトリビュート名最初の一文字
3,s,3 読み飛ばす
4,=,5 =発見。アトリビュート名確定
4,s,4 アトリビュート名に追加
5,",6 ダブルクォテイション発見。アトリビュート値に移行
5,s,5 読み飛ばす
6,",3 アトリビュート値確定。アトリビュートをリストに追加。
6,s,6 アトリビュート値文字列に追加
7,c,0 何が来ても次は0。タグが完成したのでここでいろいろする。
*/
Element* Parser::build(){
	//アトリビュートの名前と値の一時的な格納場所
	string name;
	string value;
	
	//ダミー親
	ElementData* parent = new ElementData;
	parent->mName = "<ROOT>"; //特別扱いの根元エレメント
	elements.push_back( parent ); //根元セット

	//これから作るエレメント
	ElementData* child = new ElementData;

	//終了タグですか？
	bool isEndTag = false;

	for ( int i = 0; i < mDataSize; ++i ){
		char c = mData[ i ];
		//モードで分岐
		switch ( mMode ){
			case 0: //初期状態
				switch ( c ){
					case '<': mMode = 1; break;
					default: break; //何もしない
				}
				break;
			case 1: //<の上
				switch ( c ){
					case '/': mInEndTag = true; break; //終了タグでした
					default: child->mName += c; mMode = 2; break; //エレメント名突入
				}
				break;
			case 2: //エレメント名
				if ( c == '>' ){
					mMode = 7;
				}else if ( isNormalChar( c ) ){
					child->mName += c;
				}else{
					mMode = 3; //エレメント名を抜ける
				}
				break;
			case 3: //エレメント名の後の空白
				if ( c == '>' ){
					mMode = 7;
				}else if ( isNormalChar( c ) ){
					name += c; //アトリビュート名追加
					mMode = 4;
				}else{
					; //何もしない
				}
				break;
			case 4: //アトリビュート名
				switch ( c ){
					case '=': mMode = 5; break;
					default: name += c; break;
				}
				break;
			case 5: //=の上
				switch ( c ){
					case '"': mMode = 6; break;
					default: break; //何もしない
				}
				break;
			case 6: //アトリビュート値
				switch ( c ){
					case '"': 
						mMode = 3; //エレメント名の後の空白
						//アトリビュート追加
						child->mAttributes.push_back( new Attribute( name.c_str(), value.c_str() ) );
						//名前と値を初期化
						name.clear();
						value.clear();
						break;
					default: value += c; break;
				}
				break;
			case 7: //>の上。タグ完成
				if ( mInEndTag ){ //終了タグなので、Elementリストを一段戻す
					elements.pop_back();
					parent = elements.back(); //最後のが次の親です。
					mInEndTag = false;
				}else{ //開始タグなのでエレメントを今の子供に追加
					parent->mChildren.push_back( child );
					parent = child; //新しい親はこの人です。
					child = new ElementData; //新しいのを作って準備
				}
				mMode = 0; //問答無用で0へ
				break;
		}
	}
	//めでたく材料がそろったので、Elementを作っていきます。
	Element* root = new Element;
	ElementData* rootData = elements.front();
	root->setName( rootData->mName );
	root->setAttributeNumber( rootData->mAttributes.size() );
	int j = 0;
	for ( list< Attributes* >::iterator i = rootData->mAttributes.begin(); i != rootData->mAttributes.end(); ++i ){
		root->setAttribute( j, *i );
		++j;
		*i = 0; //管理を移す
	}
	root->setChildNumber( rootData->mChildren.size() );

}

} //namespace PseudoXml
