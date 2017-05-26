#include <iostream> //これだけ許して欲しい。
using namespace std;
/*

このサンプルでは下のm以外に変数を一切つかわない。
コンパイラよりも下の世界で何が起こっているのかを思い知って欲しい。
変数を自由に作れることがどれほどありがたいことかを実感し、
また、それがどうやって可能になっているのかを感じ取ってみよう。

[メモリの使い方]

0     : 関数の引数と戻り値に使う。
1-17  : 関数の中で好きに使っていい領域とする。関数を呼ぶと壊されると考える。
18    : 8 //幅
19    : 5 //高さ
20-59 : 8x5の状態配列
60-99 : 文字列形式のステージデータ
*/

char m[ 100 ]; //メモリ。100バイトしか使わない

enum Object{
	OBJ_SPACE,
	OBJ_WALL,
	OBJ_GOAL,
	OBJ_BLOCK,
	OBJ_BLOCK_ON_GOAL,
	OBJ_MAN,
	OBJ_MAN_ON_GOAL,

	OBJ_UNKNOWN,
};

//関数プロトタイプ
void initializeGlobalVariables();
void initialize();
void draw();
void update();
void checkClear();

int main(){
	//グローバル変数初期化
	initializeGlobalVariables();

	initialize(); //ステージ初期化
	//メインループ
	while ( true ){
		//まず描画
		draw();
		//クリアチェック 結果はm[ 0 ]に入っている
		checkClear();
		if ( m[ 0 ] == 1 ){
			break; //クリアチェック
		}
		//入力取得
		cout << "a:left s:right w:up z:down. command?" << endl; //操作説明
		//入力はm[ 0 ]に入れよう
		cin >> m[ 0 ];
		//更新
		update();
	}
	//祝いのメッセージ
	cout << "Congratulation! you win." << endl;

	//Visual Studioから実行する人のために無限ループ。コマンドラインからはCtrl-Cで終えてください。
	while( true ){
		;
	} 
	return 0;
}

//---------------------以下関数定義------------------------------------------

//m[60]から始まる文字列を解釈して、m[20]から始まる状態配列を構築する関数。
void initialize(){
	m[ 0 ] = 0; //0番を読み取り側の添え字に使おう
	m[ 1 ] = 0; //1番が今のx座標
	m[ 2 ] = 0; //2番が今のy座標

	while ( m[ 60 + m[ 0 ] ] != '\0' ){ //NULL文字でない間
		//3番に物を入れる
		switch ( m[ 60 + m[ 0 ] ] ){
			case '#': m[ 3 ] = OBJ_WALL; break;
			case ' ': m[ 3 ] = OBJ_SPACE; break;
			case 'o': m[ 3 ] = OBJ_BLOCK; break;
			case 'O': m[ 3 ] = OBJ_BLOCK_ON_GOAL; break;
			case '.': m[ 3 ] = OBJ_GOAL; break;
			case 'p': m[ 3 ] = OBJ_MAN; break;
			case 'P': m[ 3 ] = OBJ_MAN_ON_GOAL; break;
			case '\n': m[ 1 ] = 0; ++m[ 2 ]; m[ 3 ] = OBJ_UNKNOWN; break; //改行処理(++y)
			default: m[ 3 ] = OBJ_UNKNOWN; break;
		}
		++m[ 0 ];
		if ( m[ 3 ] != OBJ_UNKNOWN ){ //知らない文字なら無視するのでこのif文がある
			m[ 20 + m[ 2 ]*m[ 18 ] + m[ 1 ] ] = m[ 3 ]; //書き込み m[18]は幅
			++m[ 1 ]; //++x
		}
	}
}

void draw(){
	for ( m[ 0 ] = 0; m[ 0 ] < m[ 19 ]; ++m[ 0 ] ){
		for ( m[ 1 ] = 0; m[ 1 ] < m[ 18 ]; ++m[ 1 ] ){
			m[ 2 ] = m[ 20 + m[ 0 ]*m[ 18 ] + m[ 1 ] ];
			switch ( m[ 2 ] ){
				case OBJ_SPACE : cout << ' '; break;
				case OBJ_WALL : cout << '#'; break;
				case OBJ_GOAL : cout << '.'; break;
				case OBJ_BLOCK : cout << 'o'; break;
				case OBJ_BLOCK_ON_GOAL : cout << 'O'; break;
				case OBJ_MAN : cout << 'p'; break;
				case OBJ_MAN_ON_GOAL : cout << 'P'; break;
			}
		}
		cout << endl;
	}
}

void update(){
	//移動差分に変換
	m[ 1 ] = 0; //dx
	m[ 2 ] = 0; //dy
	switch ( m[ 0 ] ){ //inputはm[100]に入れてある
		case 'a': m[ 1 ] = -1; break; //左
		case 's': m[ 1 ] = 1; break; //右
		case 'w': m[ 2 ] = -1; break; //上。Yは下がプラス
		case 'z': m[ 2 ] = 1; break; //下。
	}
	//人座標を検索
	for ( m[ 0 ] = 0; m[ 0 ] < m[ 18 ]*m[ 19 ]; ++m[ 0 ] ){
		if ( m[ 20 + m[ 0 ] ] == OBJ_MAN || m[ 20 + m[ 0 ] ] == OBJ_MAN_ON_GOAL ){
			break;
		}
	}
	m[ 3 ] = m[ 0 ] % m[ 18 ]; //xは幅で割ったあまり
	m[ 4 ] = m[ 0 ] / m[ 18 ]; //yは幅で割った商

	//移動
	//移動後座標
	m[ 5 ] = m[ 3 ] + m[ 1 ]; //tx
	m[ 6 ] = m[ 4 ] + m[ 2 ]; //ty
	//座標の最大最小チェック。外れていれば不許可
	if ( m[ 5 ] < 0 || m[ 6 ] < 0 || m[ 5 ] >= m[ 18 ] || m[ 6 ] >= m[ 19 ] ){
		return;
	}
	//A.その方向が空白またはゴール。人が移動。
	m[ 7 ] = 20 + m[ 4 ]*m[ 18 ] + m[ 3 ]; //人位置
	m[ 8 ] = 20 + m[ 6 ]*m[ 18 ] + m[ 5 ]; //移動目標位置
	if ( m[ m[ 8 ] ] == OBJ_SPACE || m[ m[ 8 ] ] == OBJ_GOAL ){ //移動先に空間があれば
		m[ m[ 8 ] ] = ( m[ m[ 8 ] ] == OBJ_GOAL ) ? OBJ_MAN_ON_GOAL : OBJ_MAN; //ゴールならゴール上の人に
		m[ m[ 7 ] ] = ( m[ m[ 7 ] ] == OBJ_MAN_ON_GOAL ) ? OBJ_GOAL : OBJ_SPACE; //もともとゴール上ならゴールに
	//B.その方向が箱。その方向の次のマスが空白またはゴールであれば移動。
	}else if ( m[ m[ 8 ] ] == OBJ_BLOCK || m[ m[ 8 ] ] == OBJ_BLOCK_ON_GOAL ){
		//2マス先が範囲内かチェック
		m[ 9 ] = m[ 5 ] + m[ 1 ];
		m[ 10 ] = m[ 6 ] + m[ 2 ];
		if ( m[ 9 ] < 0 || m[ 10 ] < 0 || m[ 9 ] >= m[ 18 ] || m[ 10 ] >= m[ 19 ] ){ //押せない
			return;
		}

		m[ 11 ] = 20 + ( m[ 6 ] + m[ 2 ] )*m[ 18 ] + ( m[ 5 ] + m[ 1 ] ); //2マス先
		if ( m[ m[ 11 ] ] == OBJ_SPACE || m[ m[ 11 ] ] == OBJ_GOAL ){
			//順次入れ替え
			m[ m[ 11 ] ] = ( m[ m[ 11 ] ] == OBJ_GOAL ) ? OBJ_BLOCK_ON_GOAL : OBJ_BLOCK;
			m[ m[ 8 ] ] = ( m[ m[ 8 ] ] == OBJ_BLOCK_ON_GOAL ) ? OBJ_MAN_ON_GOAL : OBJ_MAN;
			m[ m[ 7 ] ] = ( m[ m[ 7 ] ] == OBJ_MAN_ON_GOAL ) ? OBJ_GOAL : OBJ_SPACE;
		}
	}
}

//ブロックのみがなければクリアしている。
void checkClear(){
	for ( m[ 1 ] = 20; m[ 1 ] < 20+m[ 18 ]*m[ 19 ]; ++m[ 1 ] ){
		if ( m[ m[ 1 ] ] == OBJ_BLOCK ){
			m[ 0 ] = 0; //戻り値はm[ 0 ]
			return;
		}
	}
	m[ 0 ] = 1; //戻り値はm[ 0 ]
	return;
}


//#壁 _空間 .ゴール oブロック p人
//########\n\
//# .. p #\n\
//# oo   #\n\
//#      #\n\
//########";
void initializeGlobalVariables(){
	//幅が18
	m[ 18 ] = 8;
	//高さが19
	m[ 19 ] = 5;
	//2行目
	m[ 68 ] = '#';
	m[ 69 ] = ' ';
	m[ 70 ] = '.';
	m[ 71 ] = '.';
	m[ 72 ] = ' ';
	m[ 73 ] = 'p';
	m[ 74 ] = ' ';
	m[ 75 ] = '#';
	//3行目
	m[ 76 ] = '#';
	m[ 77 ] = ' ';
	m[ 78 ] = 'o';
	m[ 79 ] = 'o';
	m[ 80 ] = ' ';
	m[ 81 ] = ' ';
	m[ 82 ] = ' ';
	m[ 83 ] = '#';
	//4行目
	m[ 84 ] = '#';
	m[ 85 ] = ' ';
	m[ 86 ] = ' ';
	m[ 87 ] = ' ';
	m[ 88 ] = ' ';
	m[ 89 ] = ' ';
	m[ 90 ] = ' ';
	m[ 91 ] = '#';
	//1行目と5行目は全部#
	for ( m[ 0 ] = 0; m[ 0 ] < m[ 18 ]; ++m[ 0 ] ){
		m[ 60 + m[ 0 ] ] = '#';
		m[ 92 + m[ 0 ] ] = '#';
	}
}
