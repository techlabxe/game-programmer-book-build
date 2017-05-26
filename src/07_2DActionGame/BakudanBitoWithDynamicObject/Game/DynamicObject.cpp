#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"

#include "Game/DynamicObject.h"
#include "Image.h"

using namespace GameLib;

namespace{

//適当パラメータ群
//速度。単位は内部単位/フレーム
const int PLAYER_SPEED = 1000;
const int ENEMY_SPEED = 500;

} //namespace 

//できるだけ不正くさい値を入れておく。setが呼ばれないと死ぬように。
DynamicObject::DynamicObject() : 
mType( TYPE_NONE ),
mX( 0xffffffff ), 
mY( 0xffffffff ){
}

void DynamicObject::set( int x, int y, Type type ){
	//内部座標値に変換
	mX = x * 16000 + 8000;
	mY = y * 16000 + 8000;
	mType = type;
	//敵専用。移動方向初期化
	if ( mType == TYPE_ENEMY ){
		mDirectionX = mDirectionY = 0;
		Framework f = Framework::instance();
		switch ( f.getRandom( 4 ) ){
			case 0 : mDirectionX = 1; break;
			case 1 : mDirectionX = -1; break;
			case 2 : mDirectionY = 1; break;
			case 3 : mDirectionY = -1; break;
		}
	}
}

void DynamicObject::draw( const Image* image ) const {
	//内部座標を画素座標に変換(+500は四捨五入)
	int dstX = ( mX - 8000 + 500 ) / 1000;
	int dstY = ( mY - 8000 + 500 ) / 1000;
	//画像切り出し位置の同定
	int srcX, srcY;
	srcX = srcY = -1;
	switch ( mType ){
		case TYPE_1P: srcX = 0; srcY = 0; break;
		case TYPE_2P: srcX = 16; srcY = 0; break;
		case TYPE_ENEMY: srcX = 32; srcY = 16; break;
		default: HALT( "arienai" ); break;
	}
	image->draw( dstX, dstY, srcX, srcY, 16, 16 );
}

void DynamicObject::update(){
	Framework f = Framework::instance();
	//敵挙動
	if ( mType == TYPE_ENEMY ){
		//単位を追ってみよう。
		//mDirectionXは単位なし。dtはミリ秒。ENEMY_SPEEDは画素/秒。
		//この段階ではミリ秒*画素/秒
		//分母に1000をかけると分母と分子がミリ秒になって相殺。
		//画素を内部座標に変えるには1000倍する必要がある。分子と分母で1000が相殺。
		//以上から以下のようになる。
		mX += mDirectionX * ENEMY_SPEED;
		mY += mDirectionY * ENEMY_SPEED;
	}else if ( mType == TYPE_1P ){ //プレイヤー挙動
		int dx, dy;
		dx = dy = 0;
		if ( f.isKeyOn( 'w' ) ){
			dy = -1;
		}else if ( f.isKeyOn( 'z' ) ){
			dy = 1;
		}else if ( f.isKeyOn( 'a' ) ){
			dx = -1;
		}else if ( f.isKeyOn( 's' ) ){
			dx = 1;
		}
		mX += dx * PLAYER_SPEED;
		mY += dy * PLAYER_SPEED;
	}else if ( mType == TYPE_2P ){ //プレイヤー挙動
		int dx, dy;
		dx = dy = 0;
		if ( f.isKeyOn( 'i' ) ){
			dy = -1;
		}else if ( f.isKeyOn( 'm' ) ){
			dy = 1;
		}else if ( f.isKeyOn( 'j' ) ){
			dx = -1;
		}else if ( f.isKeyOn( 'k' ) ){
			dx = 1;
		}
		mX += dx * PLAYER_SPEED;
		mY += dy * PLAYER_SPEED;
	}
	//限界処理
	const int X_MIN = 8000;
	const int X_MAX = 320 * 1000 - 8000;
	const int Y_MIN = 8000;
	const int Y_MAX = 240 * 1000 - 8000;
	bool hit = false;
	if ( mX < X_MIN ){
		mX = X_MIN;
		hit = true;
	}else if ( mX > X_MAX ){
		mX = X_MAX;
		hit = true;
	}
	if ( mY < Y_MIN ){
		mY = Y_MIN;
		hit = true;
	}else if ( mY > Y_MAX ){
		mY = Y_MAX;
		hit = true;
	}
	//敵なら向き変え
	if ( hit && mType == TYPE_ENEMY ){
		mDirectionX = mDirectionY = 0;
		switch ( f.getRandom( 4 ) ){
			case 0 : mDirectionX = 1; break;
			case 1 : mDirectionX = -1; break;
			case 2 : mDirectionY = 1; break;
			case 3 : mDirectionY = -1; break;
		}
	}
}