#include <sstream>
#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Manager.h"
#include "Robo.h"
#include "Pad.h"
#include "Library/Matrix34.h"
#include "Library/GraphicsDatabase.h"
#include "Library/Model.h"
#include "GameLib/GameLibMath.h"
using namespace GameLib;
using namespace std;

//本当はファイルに出すんだよ？
const int Robo::mJumpUpTime = 20; //上昇していく時間
const int Robo::mJumpStayTime = 60; //上昇後下降までの時間
const int Robo::mJumpFallTime = 40; //下降にかかる時間
const int Robo::mMoveAccelEndCount = 30; //歩き始めて加速が終了するまでの時間
const double Robo::mMaxMoveSpeed = 0.5; //最大移動速度
const double Robo::mJumpHeight = 20.0; //最大高度
const int Robo::mCameraDelayCount = 50; //ジャンプ開始後何フレームで敵の方を向くか
const double Robo::mCameraDistanceZ = 10.0; //何メートル後ろから写す？
const double Robo::mCameraDistanceY = 4.0; //見下ろし具合
const double Robo::mCameraTargetDistanceZ = 20.0; //注視点は何メートル先？
const double Robo::mTurnSpeed = 1.0; //旋回速度

Robo::Robo( int id ) : 
mPosition( 0.0, 0.0, 0.0 ),
mAngleY( 0.0 ),
mId( id ),
mDatabase( 0 ),
mModel( 0 ),
mCameraCount( 0 ),
mCount( 0 ),
mVelocity( 0.0, 0.0, 0.0 ),
mMode( MODE_ON_LAND ){
	mDatabase = new GraphicsDatabase( "robo.txt" );
	mModel = mDatabase->createModel( "robo" );
}

Robo::~Robo(){
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
	//角度範囲補正
	if ( mAngleY > 180.0 ){
		mAngleY -= 360.0;
	}else if ( mAngleY < -180.0 ){
		mAngleY += 360.0;
	}
	++mCount;
	//ジャンプ押されてる？
	Pad* pad = Pad::instance();
	bool inputJump = pad->isOn( Pad::JUMP, mId );
	double t;
	//字句解析に似た書き方をしてみよう。コードの重複が増えるがブロック単位で見ればシンプルになる。
	//普通の書き方とどちらが良いか比べてみよう。
	switch ( mMode ){
		case MODE_JUMP_UP:
			//カメラが回りきっていないならカメラ回転継続
			if ( mCameraCount < mCameraDelayCount ){
				mAngleY += mAngleVelocityY;
				++mCameraCount;
			}
			//上昇
			t = mJumpHeight / static_cast< double >( mJumpUpTime );
			mVelocity.y = t;
			if ( !inputJump ){ //ジャンプ入力がないので下降に変化
				mMode = MODE_JUMP_FALL;
				mCount = 0;
			}else if ( mCount >= mJumpUpTime ){ //上昇終了
				mMode = MODE_JUMP_STAY; 
				mCount = 0;
			}
			mVelocity.x = mVelocity.z = 0.0; //X,Z移動を抹殺
			break;
		case MODE_JUMP_STAY:
			//カメラが回りきっていないならカメラ回転継続
			if ( mCameraCount < mCameraDelayCount ){
				mAngleY += mAngleVelocityY;
				++mCameraCount;
			}
			mVelocity.y = 0.0;
			if ( !inputJump ){ //ジャンプ入力がないので下降に変化
				mMode = MODE_JUMP_FALL;
				mCount = 0;
			}else if ( mCount >= mJumpStayTime ){ //下降へ
				mMode = MODE_JUMP_FALL;
				mCount = 0;
			}
			break;
		case MODE_JUMP_FALL:
			//カメラが回りきっていないならカメラ回転継続
			if ( mCameraCount < mCameraDelayCount ){
				mAngleY += mAngleVelocityY;
				++mCameraCount;
			}
			//下降
			t = mJumpHeight / static_cast< double >( mJumpFallTime );
			mVelocity.y = -t;
			//接地判定は最終的には衝突処理でやるのでここではやらない。
			break;
		case MODE_ON_LAND:
			if ( inputJump ){
				mMode = MODE_JUMP_UP;
				mCount = 0;
				mCameraCount = 0;

				//敵の方に向ける。
				Vector3 dir;
				dir.setSub( enemyPos, mPosition ); //自分から敵へ
				//Y軸角度はatan2( x, z )。ただし度に直すこと。
				t = GameLib::atan2( dir.x, dir.z );
				//180度以上差があれば+-360度して逆回し
				if ( t - mAngleY > 180.0 ){
					t -= 360.0;
				}else if ( mAngleY - t > 180.0 ){
					t += 360.0;
				}
				mAngleVelocityY = ( t - mAngleY ) / static_cast< double >( mCameraDelayCount );
			}else if ( pad->isOn( Pad::TURN, mId ) ){
				turn(); //コードが長くなるので関数に飛ばす
			}else{
				move(); //コードが長くなるので関数に飛ばす
			}
			mVelocity.y = 0.0;
			break;
	}
	//ここから下は衝突処理が入るとその後になる。
	mPosition += mVelocity;
	if ( mPosition.y < 0.0 ){
		mPosition.y = 0.0;
		mMode = MODE_ON_LAND;
	}
}

void Robo::turn(){
	Pad* pad = Pad::instance();
	if ( pad->isOn( Pad::LEFT, mId ) ){
		mAngleY += mTurnSpeed;
		if ( mAngleY > 180.0 ){ //-PIからPIにおさめる
			mAngleY -= 360.0;
		}
	}
	if ( pad->isOn( Pad::RIGHT, mId ) ){
		mAngleY -= mTurnSpeed;
		if ( mAngleY < -180.0 ){ //-PIからPIにおさめる
			mAngleY += 360.0;
		}
	}
}

void Robo::move(){
	//移動処理。まず視点を考慮しない加速度を出す
	Vector3 move( 0.0, 0.0, 0.0 );
	Pad* pad = Pad::instance();
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

	//今止まっているなら話は早い。適当に加速してやるだけだ
	if ( mVelocity.x == 0.0 && mVelocity.z == 0.0 ){
		//加速にかかる時間で最大速度を割れば1フレームあたりの加速度が出る。
		double accel = mMaxMoveSpeed / static_cast< double >( mMoveAccelEndCount );
		mVelocity.setMul( move, accel );
	}else{ //すでに動いている場合
		if ( move.x == 0.0 && move.z == 0.0 ){ //移動がゼロ
			mVelocity.set( 0.0, 0.0, 0.0 ); //移動はとまる。
		}else{
			//すでに動いている場合かなり面倒である。
			//45度だけ方向転換した時にゼロから加速しなおしというのはストレスだ。
			//だから、「今の速度と方向が合わない成分だけをゼロからやり直し」にする。

			//90度以上のターンなら一旦速度を0にする。
			//慣性が働く方がいいゲームもあるが、きびきび動かしたいなら慣性は邪魔だろう。
			//90度以上のターンなら現速度と加速の内積がマイナスのはずだ
			double dp = mVelocity.dot( move );
			if ( dp <= 0.0 ){
				mVelocity.set( 0.0, 0.0, 0.0 );
			}else{ //90度未満
				//現在の移動速度と水平な成分のみ取り出す
				//水平成分は、移動方向単位ベクタとの内積を、移動方向単位ベクタにかければいい。
				//移動単位ベクタE、現速度ベクタVとして、新しい速度ベクタV'すなわち平行成分は
				//V' = dot(V,E) * E
				//この時、Eは移動ベクタMを使ってE=M/|M|と書けるから、
				//V' = dot(V,M) * M / ( |M|^2 )
				//書け、単位ベクタを作る際の平方根を除ける。|M|より|M|^2の方が計算は速いのだ。
				double moveSquareLength = move.x * move.x + move.z * move.z;
				double dp = mVelocity.dot( move );
				mVelocity.setMul( move, dp / moveSquareLength );
			}
			//加速を加える。
			//移動速度は最大速度/加速時間である。
			double accel = mMaxMoveSpeed / static_cast< double >( mMoveAccelEndCount );
			mVelocity.madd( move, accel );
			//最大速度でストップ
			double speed = mVelocity.length();
			if ( speed > mMaxMoveSpeed ){
				mVelocity *= mMaxMoveSpeed / speed;
			}
		}
	}
}

void Robo::draw( const Matrix44& pvm ) const {
	//モデルに位置情報をセット
	mModel->setAngle( Vector3( 0.0, mAngleY, 0.0 ) );
	mModel->setPosition( mPosition );
	//描画
	mModel->draw( pvm );

	//デバグ表示
	if ( mId == 0 ){
		ostringstream oss;
		oss << "VEL: " << mVelocity.x << " " << mVelocity.y << " " << mVelocity.z;
		Framework().drawDebugString( 0, 0, oss.str().c_str() );
		oss.str( "" );
		oss << "ANG: " << mAngleY;
		Framework().drawDebugString( 0, 1, oss.str().c_str() );

		oss.str( "" );
		oss << "MODE: " << mMode << " COUNT:" << mCount;;
		Framework().drawDebugString( 0, 2, oss.str().c_str() );
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
	t.setMul( d, mCameraTargetDistanceZ );
	//ロボが高いところにいるならちょっと下を見てやる。これはパラメータにないその場工夫。
	t.y -= mPosition.y * 0.12; //このへんの調整も適当
	//こいつを後方にmCameraDistacneZだけ伸ばす
	Vector3 p;
	p.setMul( d, -mCameraDistanceZ );
	//YにmCameraDistanceYをプラス
	p.y += mCameraDistanceY;
	//ロボが高いところにいるならちょっと高目にして下を見てやる。これはパラメータにないその場工夫。
	p.y += mPosition.y * 0.12; //このへんの調整も適当
	//ロボ現在位置をプラス
	t += mPosition;
	p += mPosition;
	//ビュー行列作成
	vm->setViewTransform( p, t );
}