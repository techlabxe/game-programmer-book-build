#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Manager.h"

#include "Robo.h"
#include "Pad.h"
#include "Library/Matrix34.h"
#include "Library/Matrix44.h"
#include "Library/GraphicsDatabase.h"
#include "Library/Model.h"
#include "GameLib/GameLibMath.h"
using namespace GameLib;

Robo::Robo( int id ) : 
mPosition( 0.0, 0.0, 0.0 ),
mAngleY( 0.0 ),
mId( id ),
mDatabase( 0 ),
mModel( 0 ){
	mDatabase = new GraphicsDatabase( "robo.txt" );
	mModel = mDatabase->createModel( "robo" );

	Vector3 c = mPosition;
	c.y += 0.5; //中心が上にずれてる
	mSphere.setPosition( c );
	mSphere.setRadius( 0.5 );
}

Robo::~Robo(){
	SAFE_DELETE( mModel ); //使っている方が使われている方より先
	SAFE_DELETE( mDatabase );
}

const Vector3* Robo::position() const {
	return &mPosition;
}

void Robo::setPosition( const Vector3& a ){
	mPosition = a;
	Vector3 c = mPosition;
	c.y += 0.5; //中心が上にずれてる
	mSphere.setPosition( c );
}

double Robo::angleY() const {
	return mAngleY;
}

void Robo::setAngleY( double a ){
	mAngleY = a;
}

void Robo::getMove( 
Vector3* move,
const Vector3& viewVector ) const {
	//移動。視線方向を加味。
	move->set( 0.0, 0.0, 0.0 );
	Pad* pad = Pad::instance();
	if ( pad->isOn( Pad::UP, mId ) ){
		move->z = -0.2;
	}
	if ( pad->isOn( Pad::DOWN, mId ) ){
		move->z = 0.2;
	}
	if ( pad->isOn( Pad::LEFT, mId ) ){
		move->x = -0.2;
	}
	if ( pad->isOn( Pad::RIGHT, mId ) ){
		move->x = 0.2;
	}
	double ay =GameLib:: atan2( viewVector.x, viewVector.z ) + 180.0;
	Matrix34 m;
	m.setRotationY( ay );
	m.multiply( move, *move );

	//ジャンプだー
	if ( pad->isOn( Pad::JUMP, mId ) ){
		move->y += 0.1;
	}else{
		move->y -= 0.1;
	}
}

void Robo::update( 
const Vector3& move,
const Vector3& enemyPos ){
	mPosition += move;
	Vector3 c = mPosition;
	c.y += 0.5; //中心が上にずれてる
	mSphere.setPosition( c );
	//ジャンプだー(移動はgetMove()でやったので、ここでは角度)
	Pad* pad = Pad::instance();
	if ( pad->isOn( Pad::JUMP, mId ) ){
		//敵の方に向ける。
		Vector3 dir;
		dir.setSub( enemyPos, mPosition ); //自分から敵へ
		//Y軸角度はatan2( x, z )
		mAngleY = GameLib::atan2( dir.x, dir.z );
	}
}

void Robo::getDirection( Vector3* v ) const {
	Matrix34 wm;
	wm.setRotationY( mAngleY );
	wm.multiply( v, Vector3( 0.0, 0.0, 1.0 ) );
}

void Robo::draw( const Matrix44& pvm ) const {
	//モデルに位置情報をセット
	mModel->setAngle( Vector3( 0.0, mAngleY, 0.0 ) );
	mModel->setPosition( mPosition );
	//描画
	mModel->draw( pvm );
}

const Sphere* Robo::sphere() const {
	return &mSphere;
}