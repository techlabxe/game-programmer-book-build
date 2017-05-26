#ifndef INCLUDED_GAME_DYNAMIC_OBJECT_H
#define INCLUDED_GAME_DYNAMIC_OBJECT_H

class Image;
class StaticObject;

class DynamicObject{
public:
	enum Type{
		TYPE_PLAYER,
		TYPE_ENEMY,

		TYPE_NONE, //死んでます
	};
	DynamicObject();
	void set( int x, int y, Type );
	void draw( const Image* ) const;
	//移動
	void move( const int* wallsX, int* wallsY, int wallNumber );
	//どこのマスに中心があるのかたずねる。
	void getCell( int* x, int* y ) const;
	//dx,dyだけ移動した際に重なっているマス座標を配列に入れて返す。
	//引数はint[4]が二つ。戻り値は入れた数。1,2,4のどれか。
	int getIntersectionCell( int* xArray, int* yArray, int dx, int dy ) const;
	//触っている最大4マスを受け取って応答を行う。
	void doCollisionReactionToStatic( StaticObject** o, int cellNumber );
	void doCollisionReactionToDynamic( DynamicObject* );

	//DynamicObjectとの交差判定
	bool isIntersect( const DynamicObject& ) const;
	//壁用
	bool isIntersectWall( int wallCellX, int wallCellY );

	//便利関数群
	bool hasBombButtonPressed() const;	//爆弾ボタンが押されたか調べる
	bool isPlayer() const;
	bool isEnemy() const;
	void die(); //死にます(mTypeをNONEにすることで表現)
	bool isDead() const; //死んでますか？

	//直接いじりたそうなものはprivateにはしない。頻繁にStateから使うものだからだ。
	Type mType;
	//プレイヤー専用
	int mBombPower; //爆発力
	int mBombNumber; //爆弾数
	int mPlayerID; //プレイヤー番号
	//爆弾を置く人専用
	int mLastBombX[ 2 ];
	int mLastBombY[ 2 ];
private:
	//壁用
	static bool isIntersectWall( int x, int y, int wallCellX, int wallCellY );
	//今フレームの移動量を取得
	void getVelocity( int* dx, int* dy ) const;
	//移動方向を取得
	void getDirection( int* dx, int* dy ) const;
	//座標(内部単位)
	int mX;
	int mY;
	//敵専用
	int mDirectionX;
	int mDirectionY;
};

#endif
