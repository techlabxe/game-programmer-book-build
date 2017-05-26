#ifndef INCLUDED_BULLET_H
#define INCLUDED_BULLET_H

#include "GameLib/Scene/Container.h"
#include "GameLib/Scene/Model.h"

namespace GameLib{
	namespace Math{
		class Matrix44;
		class Vector3;
	}
}
using namespace GameLib::Math;

class Bullet{
public:
	Bullet();
	~Bullet();
	void create( 
		GameLib::Scene::Container,
		const char* batchName,
		const Vector3& position,
		float angleX, 
		float angleY,
		bool homing );
	const Vector3* position() const;
	void update( const Vector3& enemyPos ); 
	void draw() const;
	bool isEmpty() const; //égÇ¡ÇƒÇ»ÇØÇÍÇŒtrueÅBmCount<0Ç≈îªíË
	void die(); //íeè¡Ç¶Ç‹Ç∑
private:
	GameLib::Scene::Model mModel;
	int mCount; //-1Ç»ÇÁÇªÇÃíeÇÕãÛÇæÇ∆Ç¢Ç§Ç±Ç∆Ç…ÇµÇÊÇ§ÅB
	bool mHoming; //í«îˆÇ∑ÇÈÅH

	static const float mHomingX; //äpìxóUì±ê´î\X
	static const float mHomingY; //äpìxóUì±ê´î\Y
	static const int mLife; //íeéıñΩ
	static const float mSpeed; //íeÇÃë¨ìx
};

#endif