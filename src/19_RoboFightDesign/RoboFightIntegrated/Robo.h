#ifndef INCLUDED_ROBO_H
#define INCLUDED_ROBO_H

#include "Library/Vector3.h"

class Matrix34;
class Matrix44;
class GraphicsDatabase;
class Model;
class Bullet;

class Robo{
public:
	Robo( int id ); //番号もらう。自分が何番か知りたいので。
	~Robo();
	void draw( const Matrix44& perspectiveViewMatrix ) const;
	void update( Robo* robo );
	void setPosition( const Vector3& );
	void setAngleY( double );
	const Vector3* position() const;
	void getViewMatrix( Matrix34* ) const;
	void setDamage( int damage );
	int hitPoint() const;
private:
	void move();
	void turn();

	Vector3 mPosition;
	double mAngleY;
	int mId;
	GraphicsDatabase* mDatabase;
	Model* mModel;
	
	Bullet* mBullets;
	int mBulletNumber;

	int mCameraCount;
	int mCount; //移動開始後何フレーム経った？
	Vector3 mVelocity; //現在の平面速度
	double mAngleVelocityY; //振り向き速度

	static const int mJumpUpTime; //上昇していく時間
	static const int mJumpStayTime; //上昇後下降までの時間
	static const int mJumpFallTime; //下降にかかる時間
	static const int mMoveAccelEndCount; //歩き始めて加速が終了するまでの時間
	static const double mMaxMoveSpeed; //最大移動速度
	static const double mJumpHeight; //最大高度
	static const int mCameraDelayCount; //ジャンプ開始後何フレームで敵の方を向くか
	static const double mCameraDistanceZ; //何メートル後ろから写す？
	static const double mCameraDistanceY; //見下ろし具合
	static const double mCameraTargetDistanceZ; //注視点は何メートル先？
	static const double mTurnSpeed; //旋回速度
	static const int mMaxHitPoint; //最大ヒットポイント

	enum Mode{
		MODE_JUMP_UP, //ジャンプ上昇中
		MODE_JUMP_STAY, //ジャンプ上空で停止中
		MODE_JUMP_FALL, //ジャンプ降下中
		MODE_ON_LAND, //着地してる
	};
	Mode mMode;

	int mHitPoint; //体力
};

#endif
