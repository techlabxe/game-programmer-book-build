#ifndef INCLUDED_BULLET_H
#define INCLUDED_BULLET_H

namespace GameLib{
	namespace Math{
		class Matrix44;
		class Vector3;
	}
}
using namespace GameLib::Math;

class Model;
class GraphicsDatabase;

class Bullet{
public:
	Bullet();
	~Bullet();
	void create( 
		AutoPtr< GraphicsDatabase >&,
		const char* batchName,
		const Vector3& position,
		float angleX, 
		float angleY,
		bool homing );
	const Vector3* getPosition() const;
	void update( const Vector3& enemyPos ); 
	void draw() const;
	bool isEmpty() const; //g‚Á‚Ä‚È‚¯‚ê‚ÎtrueBmCount<0‚Å”»’è
	void die(); //’eÁ‚¦‚Ü‚·
private:
	AutoPtr< Model > mModel;
	int mCount; //-1‚È‚ç‚»‚Ì’e‚Í‹ó‚¾‚Æ‚¢‚¤‚±‚Æ‚É‚µ‚æ‚¤B
	bool mHoming; //’Ç”ö‚·‚éH

	static const float mHomingX; //Šp“x—U“±«”\X
	static const float mHomingY; //Šp“x—U“±«”\Y
	static const int mLife; //’eõ–½
	static const float mSpeed; //’e‚Ì‘¬“x
};

#endif