#ifndef INCLUDED_ROBO_H
#define INCLUDED_ROBO_H

#include "GameLib/Scene/Container.h"
#include "GameLib/Scene/Model.h"
#include "GameLib/Math/Vector3.h"
namespace GameLib{
	namespace Math{
		class Matrix34;
		class Matrix44;
	}
}
using namespace GameLib::Math;

class Bullet;

class Robo{
public:
	Robo( int id ); //番号もらう。自分が何番か知りたいので。
	~Robo();
	void draw() const;
	void update( Robo* robo );
	void setPosition( const Vector3& );
	void setAngleY( float );
	const Vector3* position() const;
	void getView( Matrix34* matrix, Vector3* position ) const;
	void setDamage( int damage );
	int getHitPoint() const;
	int getEnergy() const;
	bool getLockOn() const;
	const Vector3* getBulletPosition( int i ) const;

	static const int mJumpUpTime; //上昇していく時間
	static const int mJumpStayTime; //上昇後下降までの時間
	static const int mJumpFallTime; //下降にかかる時間
	static const int mMoveAccelEndCount; //歩き始めて加速が終了するまでの時間
	static const float mMaxMoveSpeed; //最大移動速度
	static const float mJumpHeight; //最大高度
	static const int mCameraDelayCount; //ジャンプ開始後何フレームで敵の方を向くか
	static const float mCameraDistanceZ; //何メートル後ろから写す？
	static const float mCameraDistanceY; //見下ろし具合
	static const float mCameraTargetDistanceZ; //注視点は何メートル先？
	static const float mTurnSpeed; //旋回速度
	static const int mMaxHitPoint; //最大ヒットポイントz
	static const int mMaxEnergy; //武器ポイント最大値
	static const int mEnergyPerBullet; //一発あたりの消費エネルギー
	static const int mEnergyCharge; //毎フレーム溜まるエネルギー
	static const float mLockOnAngleIn; //ロックオンする角度
	static const float mLockOnAngleOut; //ロックオンする角度
private:
	void move( bool left, bool right, bool up, bool down );
	void turn( bool left, bool right );
	//思考ルーチン。ボタン入力を返す。プレイヤー操作キャラならただ入力を取るだけ
	void think( bool* jump, bool* fire, bool* turn, bool* left, bool* right, bool* up, bool* down ) const;

	Vector3 mPosition;
	float mAngleY;
	int mId;
	GameLib::Scene::Container mSceneContainer;
	GameLib::Scene::Model mModel;
	
	Bullet* mBullets;
	int mBulletNumber;

	int mCameraCount;
	int mCount; //移動開始後何フレーム経った？
	Vector3 mVelocity; //現在の平面速度
	float mAngleVelocityY; //振り向き速度

	enum Mode{
		MODE_JUMP_UP, //ジャンプ上昇中
		MODE_JUMP_STAY, //ジャンプ上空で停止中
		MODE_JUMP_FALL, //ジャンプ降下中
		MODE_ON_LAND, //着地してる
	};
	Mode mMode;

	int mHitPoint; //体力
	int mEnergy; //武器を撃つのに必要なエネルギー
	bool mLockOn; //ロックオンしてますか？
};

#endif
