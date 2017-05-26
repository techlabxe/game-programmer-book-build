#include "GameLib/GameLib.h"
using namespace GameLib;

#include "Library/Vector3.h"
#include "Library/Matrix34.h"
#include "Library/GraphicsDatabase.h"
#include "Library/Model.h"
#include "GameLib/Math.h"
using namespace GameLib;

#include "Bullet.h"

//パラメータ群
const int Bullet::mLife = 120;
const float Bullet::mHomingX = 1.0;
const float Bullet::mHomingY = 1.0;
const float Bullet::mSpeed = 1.0;

Bullet::Bullet() : mModel( 0 ), mCount( -1 ){
}

Bullet::~Bullet(){
	if ( mModel ){
		SAFE_DELETE( mModel );
	}
}

bool Bullet::isEmpty() const {
	return ( mCount < 0 );
}

void Bullet::die(){
	mCount = -1;
}

void Bullet::create(
GraphicsDatabase* db, 
const char* batchName,
const Vector3& p,
double angleX,
double angleY,
bool homing,
const Vector3& color ){
	//モデルがあるなら消しとく
	if ( mModel ){
		SAFE_DELETE( mModel );
	}
	mModel = db->createModel( batchName );
	mModel->setPosition( p );
	mModel->setAngle( Vector3( angleX, angleY, 0.0 ) );
	mModel->setColor( color );
	mHoming = homing;
	mCount = 0;
}

const Vector3* Bullet::position() const {
	return mModel->position();
}

void Bullet::update( const Vector3& enemyPos ){
	//敵の方に向ける。
	Vector3 dir;
	Vector3 p = *mModel->position();
	Vector3 a = *mModel->angle();
	if ( mHoming ){
		dir.setSub( enemyPos, *mModel->position() ); //自分から敵へ
		//Y軸角度はGameLib::atan2( x, z )。
		double ty = GameLib::atan2( dir.x, dir.z );
		//180度以上差があれば+-360度して逆回し
		if ( ty - a.y > 180.0 ){
			ty -= 360.0;
		}else if ( a.y - ty > 180.0 ){
			ty += 360.0;
		}
		//X軸角度はY/(X,Z)。
		double zxLength = GameLib::sqrt( dir.x * dir.x + dir.z * dir.z );
		double tx = GameLib::atan2( dir.y, zxLength );
		//X軸角度はそもそも範囲が(-90,90)で180度以上離れることはない。そのままで良い。
		double hx = mHomingX; //別名
		double hy = mHomingY;
		//ホーミング範囲内ならそのものに
		if ( tx - a.x < hx && a.x - tx < hx ){
			a.x = tx;
		}else if ( tx < a.x ){
			a.x -= hx;
		}else{
			a.x += hx;
		}
		if ( ty - a.y < hy && a.y - ty < hy ){
			a.y = ty;
		}else if ( ty < a.y ){
			a.y -= hy;
		}else{
			a.y += hy;
		}
	}
	//おもろいのでz回転つけとくか
	a.z += 2.0;
	//角度更新
	mModel->setAngle( a );
	//位置はこの方向の回転行列で(0,0,1)を変換して足してやる
	Vector3 v( 0.0, 0.0, mSpeed );
	Matrix34 m;
	m.setRotationY( a.y );
	m.rotateX( -a.x );
	m.multiply( &v, v );
	p += v;
	mModel->setPosition( p );
	++mCount;
	if ( mCount >= mLife ){
		mCount = -1;
	}
}

void Bullet::draw( 
const Matrix44& pvm,
const Vector3& lightVector,
const Vector3& lightColor,
const Vector3& ambient ) const {
	mModel->draw( pvm, lightVector, lightColor, ambient );
}
