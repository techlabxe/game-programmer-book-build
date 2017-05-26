#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Functions.h"

#include "Robo.h"
#include "Bullet.h"
#include "Pad.h"
#include <sstream>
using namespace std;
using namespace GameLib;
using namespace GameLib::Scene;

//本当はファイルに出すんだよ？
const int Robo::mJumpUpTime = 20; //上昇していく時間
const int Robo::mJumpStayTime = 60; //上昇後下降までの時間
const int Robo::mJumpFallTime = 40; //下降にかかる時間
const int Robo::mMoveAccelEndCount = 30; //歩き始めて加速が終了するまでの時間
const float Robo::mMaxMoveSpeed = 0.5; //最大移動速度
const float Robo::mJumpHeight = 20.0; //最大高度
const int Robo::mCameraDelayCount = 10; //ジャンプ開始後何フレームで敵の方を向くか
const float Robo::mCameraDistanceZ = 10.0; //何メートル後ろから写す？
const float Robo::mCameraDistanceY = 4.0; //見下ろし具合
const float Robo::mCameraTargetDistanceZ = 20.0; //注視点は何メートル先？
const float Robo::mTurnSpeed = 1.0; //旋回速度
const int Robo::mMaxHitPoint = 100; //最大ヒットッポイント
const int Robo::mMaxEnergy = 100; //最大武器ポイント
const int Robo::mEnergyPerBullet = 27; //武器の消費エネルギー
const int Robo::mEnergyCharge = 1; //毎フレーム溜まるエネルギーの量
const float Robo::mLockOnAngleIn = 10.0; //ロックオンする角度
const float Robo::mLockOnAngleOut = 30.0; //ロックオンが外れる角度

Robo::Robo( int id ) : 
mPosition( 0.0, 0.0, 0.0 ),
mAngleY( 0.0 ),
mId( id ),
mBullets( 0 ),
mBulletNumber( 100 ), //きめうち
mCameraCount( 0 ),
mCount( 0 ),
mVelocity( 0.0, 0.0, 0.0 ),
mMode( MODE_ON_LAND ),
mHitPoint( mMaxHitPoint ),
mEnergy( mMaxEnergy ),
mLockOn( true ){ //向き合って始まるので最初はロックオンしてていいだろう。
	mSceneContainer = Container::create( "data/Sample_Robot.x.txt" );
	while ( !mSceneContainer.isReady() ){
		;
	}
	mModel = mSceneContainer.createModel( "batch0_0" );
	mModel.setScale( Vector3( 2.f ) );
	mBullets = new Bullet[ mBulletNumber ];
}

Robo::~Robo(){
	SAFE_DELETE_ARRAY( mBullets );
}

void Robo::setPosition( const Vector3& a ){
	mPosition = a;
}

void Robo::setAngleY( float a ){
	mAngleY = a;
}

const Vector3* Robo::getBulletPosition( int i ) const {
	if ( mBullets[ i ].isEmpty() ){
		return 0;
	}else{
		return mBullets[ i ].position();
	}
}

void Robo::update( Robo* enemy ){
	//死んでる
	if ( mHitPoint <= 0 ){
		return;
	}
	//AIの思考。プレイヤーの場合は入力を取得して返すだけ
	bool iJump;
	bool iFire;
	bool iTurn;
	bool iLeft; 
	bool iRight; 
	bool iUp;
	bool iDown; 
	think( &iJump, &iFire, &iTurn, &iLeft, &iRight, &iUp, &iDown );
	//以下もらった入力を使って行動
	const Vector3& enemyPos = *enemy->position();
	++mCount;
	//ジャンプ押されてる？
	float t;
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
			t = mJumpHeight / static_cast< float >( mJumpUpTime );
			mVelocity.y = t;
			if ( !iJump ){ //ジャンプ入力がないので下降に変化
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
			if ( !iJump ){ //ジャンプ入力がないので下降に変化
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
			t = mJumpHeight / static_cast< float >( mJumpFallTime );
			mVelocity.y = -t;
			//接地判定は最終的には衝突処理でやるのでここではやらない。
			break;
		case MODE_ON_LAND:
			if ( iJump ){
				mMode = MODE_JUMP_UP;
				mCount = 0;
				mCameraCount = 0;

				//敵の方に向ける。
				Vector3 dir;
				dir.setSub( enemyPos, mPosition ); //自分から敵へ
				//Y軸角度はatan2( x, z )。ただし度に直すこと。
				t = GameLib::Math::atan2( dir.x, dir.z );
				//180度以上差があれば+-360度して逆回し
				if ( t - mAngleY > 180.f ){
					t -= 360.f;
				}else if ( mAngleY - t > 180.f ){
					t += 360.f;
				}
				mAngleVelocityY = ( t - mAngleY ) / static_cast< float >( mCameraDelayCount );
			}else if ( iTurn ){
				turn( iLeft, iRight ); //コードが長くなるので関数に飛ばす
			}else{
				move( iLeft, iRight, iUp, iDown ); //コードが長くなるので関数に飛ばす
			}
			mVelocity.y = 0.f;
			break;
	}
	//ここから下は衝突処理が入るとその後になる。
	mPosition += mVelocity;
	if ( mPosition.y < 0.f ){
		mPosition.y = 0.f;
		mMode = MODE_ON_LAND;
	}
	//武器生成
	if ( iFire ){
		//上昇、下降中は撃てない
		if ( ( mMode != MODE_JUMP_FALL ) && ( mMode != MODE_JUMP_UP ) ){
			//エネルギー足りてる？
			if ( mEnergy >= mEnergyPerBullet ){
				//空き武器を探す
				for ( int i = 0; i < mBulletNumber; ++i ){
					if ( mBullets[ i ].isEmpty() ){
						const char* name = ( mId == 0 ) ? "bulletB" : "bulletR";
						mBullets[ i ].create(
							mSceneContainer, 
							name, 
							mPosition,
							15.f,
							mAngleY,
							mLockOn );
						mEnergy -= mEnergyPerBullet;
						break;
					}
				}
			}
		}
	}
	//武器更新
	for ( int i = 0; i < mBulletNumber; ++i ){
		if ( !mBullets[ i ].isEmpty() ){
			mBullets[ i ].update( enemyPos );
			//衝突処理
			Vector3 t;
			t.setSub( *mBullets[ i ].position(), enemyPos );
			if ( t.squareLength() < 4.f ){
				enemy->setDamage( 1 ); //1点減らしてみた。
				mBullets[ i ].die(); //弾消えます。
			}
		}
	}
	//武器エネルギーチャージ
	mEnergy += mEnergyCharge;
	if ( mEnergy > mMaxEnergy ){
		mEnergy = mMaxEnergy;
	}
	//ロックオン処理
	//まずは角度を測ってみよう。
	//角度は何で測るか？そう、内積だ。
	Vector3 toEnemy;
	toEnemy.setSub( enemyPos, mPosition );
	Vector3 myDir( 0.f, 0.f, -1.f );
	Matrix34 m;
	m.setRotationY( mAngleY + 180.f );
	m.mul( &myDir, myDir );
	toEnemy *= 1.f / toEnemy.length(); //長さを1に
	float dotProduct = toEnemy.dot( myDir );
	if ( dotProduct > 1.f ){ //誤差のせいで1を越えるかもしれない。
		dotProduct = 1.f;
	}else if ( dotProduct < -1.f ){
		dotProduct = -1.f;
	}
	//角度に直すと、
	float angle = GameLib::Math::acos( dotProduct );
	if ( mLockOn ){
		//ロックオンしてるなら外れるかどうか調べる
		if ( angle > mLockOnAngleOut ){
			mLockOn = false;
		}
	}else{
		//入るかどうか調べる
		if ( angle < mLockOnAngleIn ){
			mLockOn = true;
		}
	}
	//モデルに位置情報をセット
	mModel.setAngle( Vector3( 0.f, mAngleY, 0.f ) );
	mModel.setPosition( mPosition );
}

void Robo::think(
bool* jump,
bool* fire,
bool* turn,
bool* left,
bool* right,
bool* up,
bool* down ) const {
	Pad* pad = Pad::instance();
	if ( mId == 0 ){ //プレイヤー
		*jump = pad->isOn( Pad::JUMP, mId );
		*fire = pad->isTriggered( Pad::FIRE, mId );
		*turn = pad->isOn( Pad::TURN, mId );
		*left = pad->isOn( Pad::LEFT, mId );
		*right = pad->isOn( Pad::RIGHT, mId );
		*up = pad->isOn( Pad::UP, mId );
		*down = pad->isOn( Pad::DOWN, mId );
	}else{ //AI
		//超馬鹿
		//ロックオンしてなければジャンプする
		*jump = !mLockOn;
		//弾は撃てるだけ撃つ。
		*fire = true;
		//旋回はしない
		*turn = false;
		//移動は変数を持つのが面倒なのでしない。
		*left = *right = *up = *down = false;
	}
}

void Robo::turn( bool left, bool right ){
	if ( left ){
		mAngleY += mTurnSpeed;
		if ( mAngleY > 180.f ){ //-PIからPIにおさめる
			mAngleY -= 360.f;
		}
	}
	if ( right ){
		mAngleY -= mTurnSpeed;
		if ( mAngleY < -180.f ){ //-PIからPIにおさめる
			mAngleY += 360.0;
		}
	}
}

void Robo::move( bool left, bool right, bool up, bool down ){
	//移動処理。まず視点を考慮しない加速度を出す
	Vector3 move( 0.f, 0.f, 0.f );
	if ( up ){
		move.z = -1.f;
	}
	if ( down ){
		move.z = 1.f;
	}
	if ( left ){
		move.x = -1.f;
	}
	if ( right ){
		move.x = 1.f;
	}
	//視線方向を加味して回転
	Matrix34 m;
	m.setRotationY( mAngleY + 180.f );
	m.mul( &move, move );

	//今止まっているなら話は早い。適当に加速してやるだけだ
	if ( mVelocity.x == 0.f && mVelocity.z == 0.f ){
		//加速にかかる時間で最大速度を割れば1フレームあたりの加速度が出る。
		float accel = mMaxMoveSpeed / static_cast< float >( mMoveAccelEndCount );
		mVelocity.setMul( move, accel );
	}else{ //すでに動いている場合
		if ( move.x == 0.f && move.z == 0.f ){ //移動がゼロ
			mVelocity.set( 0.f, 0.f, 0.f ); //移動はとまる。
		}else{
			//すでに動いている場合かなり面倒である。
			//45度だけ方向転換した時にゼロから加速しなおしというのはストレスだ。
			//だから、「今の速度と方向が合わない成分だけをゼロからやり直し」にする。

			//90度以上のターンなら一旦速度を0にする。
			//慣性が働く方がいいゲームもあるが、きびきび動かしたいなら慣性は邪魔だろう。
			//90度以上のターンなら現速度と加速の内積がマイナスのはずだ
			float dp = mVelocity.dot( move );
			if ( dp <= 0.f ){
				mVelocity.set( 0.f, 0.f, 0.f );
			}else{ //90度未満
				//現在の移動速度と水平な成分のみ取り出す
				//水平成分は、移動方向単位ベクタとの内積を、移動方向単位ベクタにかければいい。
				//移動単位ベクタE、現速度ベクタVとして、新しい速度ベクタV'すなわち平行成分は
				//V' = dot(V,E) * E
				//この時、Eは移動ベクタMを使ってE=M/|M|と書けるから、
				//V' = dot(V,M) * M / ( |M|^2 )
				//書け、単位ベクタを作る際の平方根を除ける。|M|より|M|^2の方が計算は速いのだ。
				float moveSquareLength = move.x * move.x + move.z * move.z;
				float dp = mVelocity.dot( move );
				mVelocity.setMul( move, dp / moveSquareLength );
			}
			//加速を加える。
			//移動速度は最大速度/加速時間である。
			float accel = mMaxMoveSpeed / static_cast< float >( mMoveAccelEndCount );
			mVelocity.madd( move, accel );
			//最大速度でストップ
			float speed = mVelocity.length();
			if ( speed > mMaxMoveSpeed ){
				mVelocity *= mMaxMoveSpeed / speed;
			}
		}
	}
}

void Robo::draw() const {
	//描画
	mModel.draw();
	//武器
	for ( int i = 0; i < mBulletNumber; ++i ){
		if ( !mBullets[ i ].isEmpty() ){
			mBullets[ i ].draw();
		}
	}
}

const Vector3* Robo::position() const {
	return &mPosition;
}

void Robo::setDamage( int d ){
	mHitPoint -= d;
}

int Robo::getHitPoint() const {
	return mHitPoint;
}

int Robo::getEnergy() const {
	return mEnergy;
}

bool Robo::getLockOn() const {
	return mLockOn;
}

void Robo::getView( Matrix34* vm, Vector3* pos ) const {
	//まず正面方向ベクタを作成
	Vector3 d( 0.f, 0.f, 1.f );
	Matrix34 m;
	m.setRotationY( mAngleY );
	m.mul( &d, d );
	//こいつを前方にmCameraTargetDistanceZだけ伸ばす
	Vector3 t;
	t.setMul( d, mCameraTargetDistanceZ );
	//ロボが高いところにいるならちょっと下を見てやる。これはパラメータにないその場工夫。
	t.y -= mPosition.y * 0.12f; //このへんの調整も適当
	//こいつを後方にmCameraDistacneZだけ伸ばす
	Vector3 p;
	p.setMul( d, -mCameraDistanceZ );
	//YにmCameraDistanceYをプラス
	p.y += mCameraDistanceY;
	//ロボが高いところにいるならちょっと高目にして下を見てやる。これはパラメータにないその場工夫。
	p.y += mPosition.y * 0.12f; //このへんの調整も適当
	//ロボ現在位置をプラス
	t += mPosition;
	p += mPosition;
	//ビュー行列作成
	vm->setViewTransform( p, t, Vector3( 0.f, 1.f, 0.f ) );
	*pos = p;
}
