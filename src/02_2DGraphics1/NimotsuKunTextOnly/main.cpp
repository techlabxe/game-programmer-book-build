#include "GameLib/Framework.h"
using namespace GameLib;

#include <fstream>
using namespace std;

//関数プロトタイプ
void readFile( char** buffer, int* size, const char* filename );
void mainLoop();


//二次元配列クラス
//テンプレートになじみはあるだろうか？なければ基礎だけでも勉強しておこう。
//このクラス宣言の中ではTというクラスがあるかのように扱われ、
//これを使う時にはTのところにintとかboolとか入れて使う。
template< class T > class Array2D{
public:
	Array2D() : mArray( 0 ){}
	~Array2D(){
		delete[] mArray;
		mArray = 0;  //ポインタに0を入れるのはクセにしよう。
	}
	void setSize( int size0, int size1 ){
		mSize0 = size0;
		mSize1 = size1;
		mArray = new T[ size0 * size1 ];
	}
	T& operator()( int index0, int index1 ){
		return mArray[ index1 * mSize0 + index0 ];
	}
	const T& operator()( int index0, int index1 ) const {
		return mArray[ index1 * mSize0 + index0 ];
	}
private:
	T* mArray;
	int mSize0;
	int mSize1;
};

//状態クラス
class State{
public:
	State( const char* stageData, int size );
	void update( char input );
	void draw() const;
	bool hasCleared() const;
private:
	enum Object{
		OBJ_SPACE,
		OBJ_WALL,
		OBJ_BLOCK,
		OBJ_MAN,

		OBJ_UNKNOWN,
	};
	void setSize( const char* stageData, int size );

	int mWidth;
	int mHeight;
	Array2D< Object > mObjects;
	Array2D< bool > mGoalFlags;
};

//グローバル変数
State* gState = 0;

//ユーザ実装関数。中身はmainLoop()に丸投げ
namespace GameLib{
	void Framework::update(){
		mainLoop();
	}
}

void mainLoop(){
	//最初のフレームは初期化。最初の状態を描画して終わり。
	if ( !gState ){ 
		const char* filename = "stageData.txt";
		char* stageData;
		int fileSize;
		readFile( &stageData, &fileSize, filename );
		if ( !stageData ){
			cout << "stage file could not be read." << endl;
			return;
		}
		gState = new State( stageData, fileSize );
		//後始末
		delete[] stageData;
		stageData = 0;
		//初回描画
		gState->draw();
		return; //そのまま終わる
	}
	bool cleared = false;
	//メインループ
	//クリアチェック
	if ( gState->hasCleared() ){
		cleared = true;
	}
	//入力取得
	cout << "a:left s:right w:up z:down. command?" << endl; //操作説明
	char input;
	cin >> input;
	//更新
	gState->update( input );
	//描画
	gState->draw();

	if ( cleared ){
		//祝いのメッセージ
		cout << "Congratulation! you win." << endl;
		delete gState;
		gState = 0;
	}
}

//---------------------以下関数定義------------------------------------------

void readFile( char** buffer, int* size, const char* filename ){
	ifstream in( filename, ifstream::binary );
	if ( !in ){
		*buffer = 0;
		*size = 0;
	}else{
		in.seekg( 0, ifstream::end );
		*size = static_cast< int >( in.tellg() );
		in.seekg( 0, ifstream::beg );
		*buffer = new char[ *size ];
		in.read( *buffer, *size );
	}
}

State::State( const char* stageData, int size ){	
	//サイズ測定
	setSize( stageData, size );
	//配列確保
	mObjects.setSize( mWidth, mHeight );
	mGoalFlags.setSize( mWidth, mHeight );
	//初期値で埋めとく
	for ( int y = 0; y < mHeight; ++y ){
		for ( int x = 0; x < mWidth; ++x ){
			mObjects( x, y ) = OBJ_WALL; //あまった部分は壁
			mGoalFlags( x, y ) = false; //ゴールじゃない
		}
	}
	int x = 0;
	int y = 0;
	for ( int i = 0; i < size; ++i ){
		Object t;
		bool goalFlag = false;
		switch ( stageData[ i ] ){
			case '#': t = OBJ_WALL; break;
			case ' ': t = OBJ_SPACE; break;
			case 'o': t = OBJ_BLOCK; break;
			case 'O': t = OBJ_BLOCK; goalFlag = true; break;
			case '.': t = OBJ_SPACE; goalFlag = true; break;
			case 'p': t = OBJ_MAN; break;
			case 'P': t = OBJ_MAN; goalFlag = true; break;
			case '\n': x = 0; ++y; t = OBJ_UNKNOWN; break; //改行処理
			default: t = OBJ_UNKNOWN; break;
		}
		if ( t != OBJ_UNKNOWN ){ //知らない文字なら無視するのでこのif文がある
			mObjects( x, y ) = t; //書き込み
			mGoalFlags( x, y ) = goalFlag; //ゴール情報
			++x;
		}
	}
}

void State::setSize( const char* stageData, int size ){
	mWidth = mHeight = 0; //初期化
	//現在位置
	int x = 0;
	int y = 0;
	for ( int i = 0; i < size; ++i ){
		switch ( stageData[ i ] ){
			case '#': case ' ': case 'o': case 'O':
			case '.': case 'p': case 'P':
				++x;
				break;
			case '\n': 
				++y;
				//最大値更新
				mWidth = max( mWidth, x );
				mHeight = max( mHeight, y );
				x = 0; 
				break;
		}
	}
}

void State::draw() const {
	for ( int y = 0; y < mHeight; ++y ){
		for ( int x = 0; x < mWidth; ++x ){
			Object o = mObjects( x, y );
			bool goalFlag = mGoalFlags( x, y );
			if ( goalFlag ){
				switch ( o ){
					case OBJ_SPACE: cout << '.'; break;
					case OBJ_WALL: cout << '#'; break;
					case OBJ_BLOCK: cout << 'O'; break;
					case OBJ_MAN: cout << 'P'; break;
				}
			}else{
				switch ( o ){
					case OBJ_SPACE: cout << ' '; break;
					case OBJ_WALL: cout << '#'; break;
					case OBJ_BLOCK: cout << 'o'; break;
					case OBJ_MAN: cout << 'p'; break;
				}
			}
		}
		cout << endl;
	}
}

void State::update( char input ){
	//移動差分に変換
	int dx = 0;
	int dy = 0;
	switch ( input ){
		case 'a': dx = -1; break; //左
		case 's': dx = 1; break; //右
		case 'w': dy = -1; break; //上。Yは下がプラス
		case 'z': dy = 1; break; //下。
	}
	//短い変数名をつける。
	int w = mWidth;
	int h = mHeight;
	Array2D< Object >& o = mObjects;
	//人座標を検索
	int x, y;
	x = y = -1;
	bool found = false;
	for ( y = 0; y < mHeight; ++y ){
		for ( x = 0; x < mWidth; ++x ){
			if ( o( x, y ) == OBJ_MAN ){
				found = true;
				break;
			}
		}
		if ( found ){
			break;
		}
	}
	//移動
	//移動後座標
	int tx = x + dx;
	int ty = y + dy;
	//座標の最大最小チェック。外れていれば不許可
	if ( tx < 0 || ty < 0 || tx >= w || ty >= h ){
		return;
	}
	//A.その方向が空白またはゴール。人が移動。
	if ( o( tx, ty ) == OBJ_SPACE ){
		o( tx, ty ) = OBJ_MAN;
		o( x, y ) = OBJ_SPACE;
	//B.その方向が箱。その方向の次のマスが空白またはゴールであれば移動。
	}else if ( o( tx, ty ) == OBJ_BLOCK ){
		//2マス先が範囲内かチェック
		int tx2 = tx + dx;
		int ty2 = ty + dy; 
		if ( tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h ){ //押せない
			return;
		}
		if ( o( tx2, ty2 ) == OBJ_SPACE ){
			//順次入れ替え
			o( tx2, ty2 ) = OBJ_BLOCK;
			o( tx, ty ) = OBJ_MAN;
			o( x, y ) = OBJ_SPACE;
		}
	}
}

//ブロックのところのgoalFlagが一つでもfalseなら
//まだクリアしてない
bool State::hasCleared() const {
	for ( int y = 0; y < mHeight; ++y ){
		for ( int x = 0; x < mWidth; ++x ){
			if ( mObjects( x, y ) == OBJ_BLOCK ){
				if ( mGoalFlags( x, y ) == false ){
					return false;
				}
			}
		}
	}
	return true;
}

