#ifndef INCLUDED_GAME_STATIC_OBJECT_H
#define INCLUDED_GAME_STATIC_OBJECT_H

class Image;
class DynamicObject;

class StaticObject{
public:
	enum Flag{
		FLAG_WALL = ( 1 << 0 ), //ƒRƒ“ƒNƒŠ‚Å‚·
		FLAG_BRICK = ( 1 << 1 ), //ƒŒƒ“ƒK‚Å‚·
		FLAG_BOMB = ( 1 << 2 ), //”š’e‚Å‚·
		FLAG_ITEM_BOMB = ( 1 << 3 ), //”š’eƒAƒCƒeƒ€
		FLAG_ITEM_POWER = ( 1 << 4 ), //”š•—ƒAƒCƒeƒ€
		FLAG_FIRE_X = ( 1 << 5 ), //‰¡•ûŒü‰Š
		FLAG_FIRE_Y = ( 1 << 6 ), //c•ûŒü‰Š
		FLAG_EXPLODING = ( 1 << 7 ), //”š”­’†
	};
	StaticObject();

	bool checkFlag( unsigned ) const;
	void setFlag( unsigned );
	void resetFlag( unsigned );
	//°A•ÇA—ùŠ¢‚ð•`‰æ‚µA”š’e‚âƒAƒCƒeƒ€‚ª‚ ‚ê‚Î‚»‚ê‚à•`‰æ
	void draw( int x, int y, const Image* ) const;
	//”š•—‚ð•`‰æ
	void drawExplosion( int x, int y, const Image* ) const ;

	int mCount;	//‰½‚©‚ÌƒJƒEƒ“ƒg(”š’eÝ’uA”š”­AÄ‚©‚êŽn‚ß)
	//”š’eê—p
	DynamicObject* mBombOwner; //”š’e‚ÌŽ‚¿Žå
private:
	unsigned mFlags; //ƒtƒ‰ƒO
};

#endif
