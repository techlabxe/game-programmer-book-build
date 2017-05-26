#ifndef INCLUDED_ROBO_H
#define INCLUDED_ROBO_H

#include "Library/Vector3.h"
#include "Sphere.h"

class Matrix44;
class GraphicsDatabase;
class Model;

class Robo{
public:
	Robo( int id ); //番号もらう。自分が何番か知りたいので。
	~Robo();
	void draw( const Matrix44& perspectiveViewMatrix ) const;
	//動く予定をゲット
	void getMove( 
		Vector3* moveOut,
		const Vector3& viewVector ) const;
	//動き方向を与えて今フレームの処理をさせる
	void update( 
		const Vector3& move,
		const Vector3& enemyPosition );
	const Vector3* position() const;
	double angleY() const;
	void setPosition( const Vector3& );
	void setAngleY( double );
	void getDirection( Vector3* ) const;
	const Sphere* sphere() const;
private:
	Vector3 mPosition;
	double mAngleY;
	int mId;
	GraphicsDatabase* mDatabase;
	Model* mModel;
	Sphere mSphere;
};

#endif
