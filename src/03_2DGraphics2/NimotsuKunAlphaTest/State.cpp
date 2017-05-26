#include "State.h"
#include "Image.h"

State::State( const char* stageData, int size ) : mImage( 0 ){	
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
	//画像読み込み
	mImage = new Image( "nimotsuKunImage2.dds" );
}

State::~State(){
	delete mImage;
	mImage = 0;
}

void State::setSize( const char* stageData, int size ){
	const char* d = stageData; //読み込みポインタ
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
				mWidth = ( mWidth > x ) ? mWidth : x;
				mHeight = ( mHeight > y ) ? mHeight : y;
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
			//壁以外なら床を描く
			if ( o != OBJ_WALL ){
				if ( goalFlag ){
					drawCell( x, y, IMAGE_ID_GOAL );
				}else{
					drawCell( x, y, IMAGE_ID_SPACE );
				}
			}
			ImageID id = IMAGE_ID_SPACE;
			switch ( o ){
				case OBJ_WALL: id = IMAGE_ID_WALL; break;
				case OBJ_BLOCK: id = IMAGE_ID_BLOCK; break;
				case OBJ_MAN: id = IMAGE_ID_PLAYER; break;
			}
			if ( id != IMAGE_ID_SPACE ){ //床はもう描いたので不要
				drawCell( x, y, id );
			}
		}
	}
}

void State::drawCell( int x, int y, ImageID id ) const {
	mImage->draw( x*32, y*32, id*32, 0, 32, 32 );
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
