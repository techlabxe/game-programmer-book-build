#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;

#include "Game/DynamicObject.h"
#include "Game/StaticObject.h"
#include "Image.h"
#include "Pad.h"

namespace {

//適当パラメータ群
//速度。単位は内部単位/フレーム
static const int PLAYER_SPEED = 1000;
static const int ENEMY_SPEED = 500;
static const int HALF_SIZE = 6000;

//内部単位へ
int convertCellToInner( int x ){
	return x * 16000 + 8000;
}
//内部単位から画素単位へ
int convertInnerToPixel( int x ){
	return  ( x - 8000 + 500 ) / 1000;
}

} //namespace{}

//できるだけ不正くさい値を入れておく。setが呼ばれないと死ぬように。
DynamicObject::DynamicObject() : 
mType( TYPE_NONE ),
mBombPower( 10 ),
mBombNumber( 10 ),
mPlayerID( 0xffffffff ),
mX( -1 ),
mY( -1 ),
mDirectionX( 0 ),
mDirectionY( 0 ){
	mLastBombX[ 0 ] = mLastBombX[ 1 ] = -1;
	mLastBombY[ 0 ] = mLastBombY[ 1 ] = -1;
}

void DynamicObject::set( int x, int y, Type type ){
	//内部座標値に変換
	mX = convertCellToInner( x );
	mY = convertCellToInner( y );
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
	if ( isDead() ){
		return;
	}
	//内部座標を画素座標に変換(+500は四捨五入)
	int dstX = convertInnerToPixel( mX );
	int dstY = convertInnerToPixel( mY );
	//画像切り出し位置の同定
	int srcX, srcY;
	srcX = srcY = -1;
	switch ( mType ){
		case TYPE_PLAYER:
			switch ( mPlayerID ){
				case 0: srcX = 0; srcY = 0; break;
				case 1: srcX = 16; srcY = 0; break;
			}
			break;
		case TYPE_ENEMY: srcX = 32; srcY = 16; break;
		default: HALT( "arienai" ); break;
	}
	image->draw( dstX, dstY, srcX, srcY, 16, 16 );
}

void DynamicObject::move( const int* wallsX, int* wallsY, int wallNumber ){
	//移動量取得
	int dx, dy;
	getVelocity( &dx, &dy );
	
	//X,Y別々に移動した時に当たるかチェック
	int movedX = mX + dx;
	int movedY = mY + dy;
	bool hitX = false;
	bool hitY = false;
	bool hit = false;
	for ( int i = 0; i < wallNumber; ++i ){
		if ( isIntersectWall( movedX, mY, wallsX[ i ], wallsY[ i ] ) ){
			hitX = hit = true;
		}
		if (  isIntersectWall( mX, movedY, wallsX[ i ], wallsY[ i ] ) ){
			hitY = hit = true;
		}
	}
	if ( hitX && !hitY ){
		mY = movedY; //Yのみオーケー
	}else if ( !hitX && hitY ){
		mX = movedX; //Xのみオーケー
	}else{ //ダメなので普通に
		for ( int i = 0; i < wallNumber; ++i ){
			if ( isIntersectWall( movedX, movedY, wallsX[ i ], wallsY[ i ] ) ){
				hit = true;
			}
		}
		if ( !hit ){
			mX = movedX;
			mY = movedY;
		}
	}
	//敵なら向き変え
	if ( hit && mType == TYPE_ENEMY ){
		mDirectionX = mDirectionY = 0;
		switch ( Framework::instance().getRandom( 4 ) ){
			case 0 : mDirectionX = 1; break;
			case 1 : mDirectionX = -1; break;
			case 2 : mDirectionY = 1; break;
			case 3 : mDirectionY = -1; break;
		}
	}
}

bool DynamicObject::isIntersectWall( int x, int y, int wallX, int wallY ){
	int wx = convertCellToInner( wallX );
	int wy = convertCellToInner( wallY );

	int al = x - HALF_SIZE; //left A
	int ar = x + HALF_SIZE; //right A
	int bl = wx - 8000; //left B
	int br = wx + 8000; //right B
	if ( ( al < br ) && ( ar > bl ) ){
		int at = y - HALF_SIZE; //top A
		int ab = y + HALF_SIZE; //bottom A
		int bt = wy - 8000; //top B
		int bb = wy + 8000; //bottom B
		if ( ( at < bb ) && ( ab > bt ) ){
			return true;
		}
	}
	return false;
}

bool DynamicObject::isIntersectWall( int wallX, int wallY ){
	return isIntersectWall( mX, mY, wallX, wallY );
}

void DynamicObject::getVelocity( int* dx, int* dy ) const {
	//スピードを変数に格納
	int speedX, speedY;
	if ( mType == TYPE_ENEMY ){
		speedX = ENEMY_SPEED;
		speedY = ENEMY_SPEED;
	}else{
		speedX = PLAYER_SPEED;
		speedY = PLAYER_SPEED;
	}
	//向き取得
	getDirection( dx, dy );
	//速度計算q
	*dx = *dx * speedX;
	*dy = *dy * speedY;
}

void DynamicObject::getDirection( int* dx, int* dy ) const {
	Framework f = Framework::instance();
	*dx = *dy = 0;
	if ( mType == TYPE_PLAYER ){
		if ( Pad::isOn( Pad::L, mPlayerID ) ){
			*dx = -1;
		}else if ( Pad::isOn( Pad::R, mPlayerID ) ){
			*dx = 1;
		}
		if ( Pad::isOn( Pad::U, mPlayerID ) ){
			*dy = -1;
		}else if ( Pad::isOn( Pad::D, mPlayerID ) ){
			*dy = 1;
		}
	}else if ( mType == TYPE_ENEMY ){
		*dx = mDirectionX;
		*dy = mDirectionY;
	}
}

void DynamicObject::doCollisionReactionToDynamic( DynamicObject* another ){
	//相手が死んでいれば無視
	if ( another->isDead() ){
		return;
	}
	//対称に書くために別名に格納
	DynamicObject& o1 = *this;
	DynamicObject& o2 = *another;
	if ( o1.isIntersect( o2 ) ){ //衝突判定
		//プレイヤーを殺す
		if ( o1.isPlayer() && o2.isEnemy() ){
			o1.die();
		}else if ( o1.isEnemy() && o2.isPlayer() ){
			o2.die();
		}
	}
}

bool DynamicObject::isIntersect( const DynamicObject& o ) const {
	int al = mX - HALF_SIZE; //left A
	int ar = mX + HALF_SIZE; //right A
	int bl = o.mX - HALF_SIZE; //left B
	int br = o.mX + HALF_SIZE; //right B
	if ( ( al < br ) && ( ar > bl ) ){
		int at = mY - HALF_SIZE; //top A
		int ab = mY + HALF_SIZE; //bottom A
		int bt = o.mY - HALF_SIZE; //top B
		int bb = o.mY + HALF_SIZE; //bottom B
		if ( ( at < bb ) && ( ab > bt ) ){
			return true;
		}
	}
	return false;

}

bool DynamicObject::hasBombButtonPressed() const {
	if ( mType == TYPE_PLAYER ){
		return Pad::isOn( Pad::A, mPlayerID );
	}else{
		return false;
	}
}

void DynamicObject::getCell( int* x, int* y ) const {
	*x = mX / 16000;
	*y = mY / 16000;
}

bool DynamicObject::isPlayer() const {
	return ( mType == TYPE_PLAYER );
}

bool DynamicObject::isEnemy() const {
	return ( mType == TYPE_ENEMY );
}

void DynamicObject::die(){
	mType = TYPE_NONE;
}

bool DynamicObject::isDead() const {
	return ( mType == TYPE_NONE );
}
