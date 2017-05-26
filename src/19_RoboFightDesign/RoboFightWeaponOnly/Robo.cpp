#include <sstream>
#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Manager.h"

#include "Robo.h"
#include "Bullet.h"
#include "Pad.h"
#include "Library/Matrix34.h"
#include "Library/GraphicsDatabase.h"
#include "Library/Model.h"
#include "GameLib/Math.h"
using namespace GameLib;

Robo::Robo( int id ) : 
mPosition( 0.0, 0.0, 0.0 ),
mAngleY( 0.0 ),
mId( id ),
mDatabase( 0 ),
mModel( 0 ),
mBullets( 0 ),
mBulletNumber( 100 ){ //きめうち
	mDatabase = new GraphicsDatabase( "robo.txt" );
	mModel = mDatabase->createModel( "robo" );
	mBullets = new Bullet[ mBulletNumber ];
}

Robo::~Robo(){
	SAFE_DELETE_ARRAY( mBullets );
	SAFE_DELETE( mModel ); //使っている方が使われている方より先
	SAFE_DELETE( mDatabase );
}

void Robo::setPosition( const Vector3& a ){
	mPosition = a;
}

void Robo::setAngleY( double a ){
	mAngleY = a;
}

void Robo::update( const Vector3& enemyPos ){
	Pad* pad = Pad::instance();
	if ( pad->isOn( Pad::TURN, mId ) ){
		if ( pad->isOn( Pad::LEFT, mId ) ){
			mAngleY += 1.0;
			if ( mAngleY > 180.0 ){ //-PIからPIにおさめる
				mAngleY -= 360.0;
			}
		}
		if ( pad->isOn( Pad::RIGHT, mId ) ){
			mAngleY -= 1.0;
			if ( mAngleY < -180.0 ){ //-PIからPIにおさめる
				mAngleY += 360.0;
			}
		}
	}else{
		//移動処理。まず視点を考慮しない加速度を出す
		Vector3 move( 0.0, 0.0, 0.0 );
		if ( pad->isOn( Pad::UP, mId ) ){
			move.z = -1.0;
		}
		if ( pad->isOn( Pad::DOWN, mId ) ){
			move.z = 1.0;
		}
		if ( pad->isOn( Pad::LEFT, mId ) ){
			move.x = -1.0;
		}
		if ( pad->isOn( Pad::RIGHT, mId ) ){
			move.x = 1.0;
		}
		//視線方向を加味して回転
		Matrix34 m;
		m.setRotationY( mAngleY + 180.0 );
		m.multiply( &move, move );
		mPosition += move;
	}

	//武器生成
	if ( pad->isOn( Pad::FIRE, mId ) ){
		//空き武器を探す
		for ( int i = 0; i < mBulletNumber; ++i ){
			if ( mBullets[ i ].isEmpty() ){
				mBullets[ i ].create(
					mDatabase, 
					"bullet", 
					mPosition,
					45.0,
					mAngleY );
				break;
			}
		}
	}
	//武器更新
	for ( int i = 0; i < mBulletNumber; ++i ){
		if ( !mBullets[ i ].isEmpty() ){
			mBullets[ i ].update( enemyPos );
		}
	}
}

void Robo::draw( const Matrix44& pvm ) const {
	//モデルに位置情報をセット
	mModel->setAngle( Vector3( 0.0, mAngleY, 0.0 ) );
	mModel->setPosition( mPosition );
	//描画
	mModel->draw( pvm );
	//武器
	for ( int i = 0; i < mBulletNumber; ++i ){
		if ( !mBullets[ i ].isEmpty() ){
			mBullets[ i ].draw( pvm );
		}
	}
}

const Vector3* Robo::position() const {
	return &mPosition;
}

void Robo::getViewMatrix( Matrix34* vm ) const {
	//まず正面方向ベクタを作成
	Vector3 d( 0.0, 0.0, 1.0 );
	Matrix34 m;
	m.setRotationY( mAngleY );
	m.multiply( &d, d );
	//こいつを前方にmCameraTargetDistanceZだけ伸ばす
	Vector3 t;
	t.setMul( d, 20.0 );
	//ロボが高いところにいるならちょっと下を見てやる。これはパラメータにないその場工夫。
	t.y -= mPosition.y * 0.12; //このへんの調整も適当
	//こいつを後方にmCameraDistacneZだけ伸ばす
	Vector3 p;
	p.setMul( d, -20.0 );
	//YにmCameraDistanceYをプラス
	p.y += 20.0;
	//ロボが高いところにいるならちょっと高目にして下を見てやる。これはパラメータにないその場工夫。
	p.y += mPosition.y * 0.12; //このへんの調整も適当
	//ロボ現在位置をプラス
	t += mPosition;
	p += mPosition;
	//ビュー行列作成
	vm->setViewTransform( p, t );
}