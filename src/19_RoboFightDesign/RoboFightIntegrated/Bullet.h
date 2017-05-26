#ifndef INCLUDED_BULLET_H
#define INCLUDED_BULLET_H

class Model;
class GraphicsDatabase;
class Matrix44;
class Vector3;

class Bullet{
public:
	Bullet();
	~Bullet();
	void create( 
		GraphicsDatabase*,
		const char* batchName,
		const Vector3& position,
		double angleX, double angleY );
	const Vector3* position() const;
	void update( const Vector3& enemyPos );
	void draw( const Matrix44& projectionVewMatrix ) const;
	bool isEmpty() const; //égÇ¡ÇƒÇ»ÇØÇÍÇŒtrueÅBmCount<0Ç≈îªíË
	void die(); //íeè¡Ç¶Ç‹Ç∑
private:
	Model* mModel;
	int mCount; //-1Ç»ÇÁÇªÇÃíeÇÕãÛÇæÇ∆Ç¢Ç§Ç±Ç∆Ç…ÇµÇÊÇ§ÅB

	static const float mHomingX; //äpìxóUì±ê´î\X
	static const float mHomingY; //äpìxóUì±ê´î\Y
	static const int mLife; //íeéıñΩ
	static const float mSpeed; //íeÇÃë¨ìx
};

#endif