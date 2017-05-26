#include <iostream>
#include <fstream>
using namespace std;

//関数プロトタイプ
void readFile( char** buffer, int* size, const char* filename );

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

		OBJ_GOAL_FLAG = ( 1 << 7 ), //ゴールフラグ
	};
	void setSize( const char* stageData, int size );

	int mWidth;
	int mHeight;
	Array2D< unsigned char > mObjects; //ビット演算するのでunsigned char。ここまでけちらなくてもいいが。
};

int main( int argc, char** argv ){
	const char* filename = "stageData.txt";
	if ( argc >= 2 ){
		filename = argv[ 1 ];
	}
	char* stageData;
	int fileSize;
	readFile( &stageData, &fileSize, filename );
	if ( !stageData ){
		cout << "stage file could not be read." << endl;
		return 1;
	}
	State* state = new State( stageData, fileSize );

	//メインループ
	while ( true ){
		//まず描画
		state->draw();
		//クリアチェック
		if ( state->hasCleared() ){
			break; //クリアチェック
		}
		//入力取得
		cout << "a:left s:right w:up z:down. command?" << endl; //操作説明
		char input;
		cin >> input;
		//更新
		state->update( input ); 	
	}
	//祝いのメッセージ
	cout << "Congratulation's! you won." << endl;
	//後始末
	delete[] stageData;
	stageData = 0;

	//無限ループ(ctrl-C以外で勝手に終わらないためのもの)
	while ( true ){
		;
	}
	return 0;
}

//---------------------以下関数定義------------------------------------------

void readFile( char** buffer, int* size, const char* filename ){
	ifstream in( filename );
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
	//初期値で埋めとく
	for ( int y = 0; y < mHeight; ++y ){
		for ( int x = 0; x < mWidth; ++x ){
			mObjects( x, y ) = OBJ_WALL; //あまった部分は壁
		}
	}
	int x = 0;
	int y = 0;
	for ( int i = 0; i < size; ++i ){
		unsigned char t;
		switch ( stageData[ i ] ){
			case '#': t = OBJ_WALL; break;
			case ' ': t = OBJ_SPACE; break;
			case 'o': t = OBJ_BLOCK; break;
			case 'O': t = OBJ_BLOCK | OBJ_GOAL_FLAG; break;
			case '.': t = OBJ_SPACE | OBJ_GOAL_FLAG; break;
			case 'p': t = OBJ_MAN; break;
			case 'P': t = OBJ_MAN | OBJ_GOAL_FLAG; break;
			case '\n': x = 0; ++y; t = OBJ_UNKNOWN; break; //改行処理
			default: t = OBJ_UNKNOWN; break;
		}
		if ( t != OBJ_UNKNOWN ){ //知らない文字なら無視するのでこのif文がある
			mObjects( x, y ) = t; //書き込み
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
			switch ( mObjects( x, y ) ){
				case ( OBJ_SPACE | OBJ_GOAL_FLAG ): cout << '.'; break;
				case ( OBJ_WALL | OBJ_GOAL_FLAG ): cout << '#'; break;
				case ( OBJ_BLOCK | OBJ_GOAL_FLAG ): cout << 'O'; break;
				case ( OBJ_MAN | OBJ_GOAL_FLAG ): cout << 'P'; break;
				case OBJ_SPACE: cout << ' '; break;
				case OBJ_WALL: cout << '#'; break;
				case OBJ_BLOCK: cout << 'o'; break;
				case OBJ_MAN: cout << 'p'; break;
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
	//短い変数名をつける。参照を無理やり使ってみた。const付きであることに注意
	const int& w = mWidth;
	const int& h = mHeight;
	Array2D< unsigned char >& o = mObjects;
	//人座標を検索
	int x, y;
	x = y = -1; //危険な値
	bool found = false;
	for ( y = 0; y < h; ++y ){
		for ( x = 0; x < w; ++x ){
			if ( ( o( x, y ) & ~OBJ_GOAL_FLAG ) == OBJ_MAN ){ //ゴールフラグ以外を取り出すための& ~OBJ_GOAL_FLAG
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
	if ( ( o( tx, ty ) & ~OBJ_GOAL_FLAG ) == OBJ_SPACE ){
		o( tx, ty ) = ( o( tx, ty ) & OBJ_GOAL_FLAG ) | OBJ_MAN; //ゴールフラグを保存するためにこんな面倒なことが必要。以下もほぼ同じ
		o( x, y ) = ( o( x, y ) & OBJ_GOAL_FLAG ) | OBJ_SPACE;
	//B.その方向が箱。その方向の次のマスが空白またはゴールであれば移動。
	}else if ( o( tx, ty ) == OBJ_BLOCK ){
		//2マス先が範囲内かチェック
		int tx2 = tx + dx;
		int ty2 = ty + dy; 
		if ( tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h ){ //押せない
			return;
		}
		if ( ( o( tx2, ty2 ) & ~OBJ_GOAL_FLAG ) == OBJ_SPACE ){
			//順次入れ替え
			o( tx2, ty2 ) = ( o( tx2, ty2 ) & OBJ_GOAL_FLAG ) | OBJ_BLOCK;
			o( tx, ty ) = ( o( tx, ty ) & OBJ_GOAL_FLAG ) | OBJ_MAN;
			o( x, y ) = ( o( x, y ) & OBJ_GOAL_FLAG ) | OBJ_SPACE;
		}
	}
}

//ブロックのところのgoalFlagが一つでもfalseなら
//まだクリアしてない
bool State::hasCleared() const {
	for ( int y = 0; y < mHeight; ++y ){
		for ( int x = 0; x < mWidth; ++x ){
			if ( mObjects( x, y ) == OBJ_BLOCK ){ //ゴールフラグがないただのブロック
				return false;
			}
		}
	}
	return true;
}

