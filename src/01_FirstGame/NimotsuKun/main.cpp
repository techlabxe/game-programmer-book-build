#include <iostream>
using namespace std;

//#壁 _空間 .ゴール oブロック p人
const char gStageData[] = "\
########\n\
# .. p #\n\
# oo   #\n\
#      #\n\
########";
const int gStageWidth = 8;
const int gStageHeight = 5;

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
void initialize( Object* state, int w, int h, const char* stageData );
void draw( const Object* state, int w, int h );
void update( Object* state, char input, int w, int h );
bool checkClear( const Object* state, int w, int h );

int main(){
	//一次元配列である理由は本文参照
	Object* state = new Object[ gStageWidth * gStageHeight ]; //状態配列確保

	initialize( state, gStageWidth, gStageHeight, gStageData ); //ステージ初期化
	//メインループ
	while ( true ){
		//まず描画
		draw( state, gStageWidth, gStageHeight );
		//クリアチェック
		if ( checkClear(state, gStageWidth, gStageHeight ) ){
			break; //クリアチェック
		}
		//入力取得
		cout << "a:left s:right w:up z:down. command?" << endl; //操作説明
		char input;
		cin >> input;
		//更新
		update( state, input, gStageWidth, gStageHeight ); 	
	}
	//祝いのメッセージ
	cout << "Congratulation's! you won." << endl;
	//後始末
	delete[] state;
	state = 0;

	//Visual Studioから実行する人のために無限ループ。コマンドラインからはCtrl-Cで終えてください。
	while( true ){
		;
	} 
	return 0;
}

//---------------------以下関数定義------------------------------------------


//いつか使う日も来るだろうと高さも渡す仕様にしたが、現状使っていないので名前だけ(height)コメントアウトしてある。
void initialize( Object* state, int width, int /* height */, const char* stageData ){	
	const char* d = stageData; //読み込みポインタ
	int x = 0;
	int y = 0;
	while ( *d != '\0' ){ //NULL文字でない間
		Object t; //特に意味はないが使う回数が多い変数に私は良くtを使う。temporaryの略。たぶんよくない習慣だが、無駄に長い名前にして読みにくいのも困り物だろう。
		switch ( *d ){
			case '#': t = OBJ_WALL; break;
			case ' ': t = OBJ_SPACE; break;
			case 'o': t = OBJ_BLOCK; break;
			case 'O': t = OBJ_BLOCK_ON_GOAL; break;
			case '.': t = OBJ_GOAL; break;
			case 'p': t = OBJ_MAN; break;
			case 'P': t = OBJ_MAN_ON_GOAL; break;
			case '\n': x = 0; ++y; t = OBJ_UNKNOWN; break; //改行処理
			default: t = OBJ_UNKNOWN; break;
		}
		++d;
		if ( t != OBJ_UNKNOWN ){ //知らない文字なら無視するのでこのif文がある
			state[ y*width + x ] = t; //書き込み
			++x;
		}
	}
}

void draw( const Object* state, int width, int height ){
	const char font[] = {' ', '#', '.', 'o', 'O', 'p', 'P'}; //Object列挙の順
	for ( int y = 0; y < height; ++y ){
		for ( int x=0; x < width; ++x ){
			Object o = state[ y*width + x ];
			cout << font[ o ];
		}
		cout << endl;
	}
}

//第一引数はほかの関数ではstateとしているが、あまりに頻繁に使うので
//短いsで済ませている。w,hもそれぞれwidth,heightである。
void update( Object* s, char input, int w, int h ){
	//移動差分に変換(dはdifferenceでもdeltaでもお好きな方の略だと思って欲しい)
	int dx = 0; 
	int dy = 0;
	switch ( input ){
		case 'a': dx = -1; break; //左
		case 's': dx = 1; break; //右
		case 'w': dy = -1; break; //上。Yは下がプラス
		case 'z': dy = 1; break; //下。
	}
	//人座標を検索
	int i = -1;
	for ( i = 0; i < w * h; ++i ){
		if ( s[ i ] == OBJ_MAN || s[ i ] == OBJ_MAN_ON_GOAL ){
			break;
		}
	}
	int x = i % w; //xは幅で割ったあまり
	int y = i / w; //yは幅で割った商

	//移動
	//移動後座標(tに意味はない。ごめんなさい)
	int tx = x + dx;
	int ty = y + dy;
	//座標の最大最小チェック。外れていれば不許可
	if ( tx < 0 || ty < 0 || tx >= w || ty >= h ){
		return;
	}
	//A.その方向が空白またはゴール。人が移動。
	int p = y*w + x; //人位置
	int tp = ty*w + tx; //ターゲット位置(TargetPosition)
	if ( s[ tp ] == OBJ_SPACE || s[ tp ] == OBJ_GOAL ){
		s[ tp ] = ( s[ tp ] == OBJ_GOAL ) ? OBJ_MAN_ON_GOAL : OBJ_MAN; //ゴールならゴール上の人に
		s[ p ] = ( s[ p ] == OBJ_MAN_ON_GOAL ) ? OBJ_GOAL : OBJ_SPACE; //もともとゴール上ならゴールに
	//B.その方向が箱。その方向の次のマスが空白またはゴールであれば移動。
	}else if ( s[ tp ] == OBJ_BLOCK || s[ tp ] == OBJ_BLOCK_ON_GOAL ){
		//2マス先が範囲内かチェック
		int tx2 = tx + dx;
		int ty2 = ty + dy; 
		if ( tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h ){ //押せない
			return;
		}

		int tp2 = ( ty + dy )*w + ( tx + dx ); //2マス先
		if ( s[ tp2 ] == OBJ_SPACE || s[ tp2 ] == OBJ_GOAL ){
			//順次入れ替え
			s[ tp2 ] = ( s[ tp2 ] == OBJ_GOAL ) ? OBJ_BLOCK_ON_GOAL : OBJ_BLOCK;
			s[ tp ] = ( s[ tp ] == OBJ_BLOCK_ON_GOAL ) ? OBJ_MAN_ON_GOAL : OBJ_MAN;
			s[ p ] = ( s[ p ] == OBJ_MAN_ON_GOAL ) ? OBJ_GOAL : OBJ_SPACE;
		}
	}
}

//ブロックのみがなければクリアしている。
bool checkClear( const Object* s, int width, int height ){
	for ( int i = 0; i < width*height; ++i ){
		if ( s[ i ] == OBJ_BLOCK ){
			return false;
		}
	}
	return true;
}
