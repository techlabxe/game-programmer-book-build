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
		double angleX, 
		double angleY,
		bool homing,
		unsigned color );
	const Vector3* position() const;
	//‘æ“ñˆø”‚Í’Ç”ö‚·‚é‚©”Û‚©
	void update( const Vector3& enemyPos ); 
	void draw( const Matrix44& projectionVewMatrix ) const;
	bool isEmpty() const; //g‚Á‚Ä‚È‚¯‚ê‚ÎtrueBmCount<0‚Å”»’è
	void die(); //’eÁ‚¦‚Ü‚·
private:
	Model* mModel;
	int mCount; //-1‚È‚ç‚»‚Ì’e‚Í‹ó‚¾‚Æ‚¢‚¤‚±‚Æ‚É‚µ‚æ‚¤B
	bool mHoming; //’Ç”ö‚·‚éH

	static const float mHomingX; //Šp“x—U“±«”\X
	static const float mHomingY; //Šp“x—U“±«”\Y
	static const int mLife; //’eõ–½
	static const float mSpeed; //’e‚Ì‘¬“x
};

#endif