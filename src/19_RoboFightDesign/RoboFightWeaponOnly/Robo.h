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
	Robo( int id ); //î‘çÜÇ‡ÇÁÇ§ÅBé©ï™Ç™âΩî‘Ç©ímÇËÇΩÇ¢ÇÃÇ≈ÅB
	~Robo();
	void draw( const Matrix44& perspectiveViewMatrix ) const;
	void update( const Vector3& enemyPosition );
	void setPosition( const Vector3& );
	void setAngleY( double );
	const Vector3* position() const;
	void getViewMatrix( Matrix34* ) const;
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
};

#endif
