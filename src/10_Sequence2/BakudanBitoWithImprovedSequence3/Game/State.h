#ifndef INCLUDED_GAME_STATE_H
#define INCLUDED_GAME_STATE_H

#include "Array2D.h"

class Image; //宣言だけしておけばいい。インクルードしなくていい。
class StaticObject;
class DynamicObject;

class State{
public:
	//ステージ番号を与えてコンストラクト。0だと二人用
	static const int STAGE_ID_2PLAYERS = 0;
	State( int stageID );
	~State();
	void update();
	void draw() const;
	//外から取る情報
	bool hasCleared() const;
	bool isAlive( int playerID ) const; //生きてる？
private:
	//炎設置。座標は爆弾の座標
	void setFire( int x, int y );

	//動かないオブジェクト
	Array2D< StaticObject > mStaticObjects;
	//動くオブジェクト
	DynamicObject* mDynamicObjects;
	int mDynamicObjectNumber;

	int mStageID;

	Image* mImage; //画像
};

#endif
