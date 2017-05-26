#ifndef INCLUDED_GAME_STATIC_OBJECT_H
#define INCLUDED_GAME_STATIC_OBJECT_H

class Image;

class StaticObject{
public:
	enum Flag{
		FLAG_WALL = ( 1 << 0 ), //コンクリです
		FLAG_BRICK = ( 1 << 1 ), //レンガです
		FLAG_ITEM_BOMB = ( 1 << 2 ), //爆弾アイテム
		FLAG_ITEM_POWER = ( 1 << 3 ), //爆風アイテム
	};
	StaticObject();

	//床、壁、煉瓦を描画し、爆弾やアイテムがあればそれも描画
	void draw( int x, int y, const Image* ) const;

	bool checkFlag( unsigned ) const;
	void setFlag( unsigned );
	void resetFlag( unsigned );

private:
	unsigned mFlags;
};

#endif
