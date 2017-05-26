#include "GameLib/Framework.h"

#include "Game/State.h"
#include "Game/StaticObject.h"

#include "Image.h"

using namespace std;
using namespace GameLib;

namespace{ //名前なし名前空間

//マップの広さ
const int WIDTH = 19;
const int HEIGHT = 15;

//適当ステージデータ
struct StageData{
	int mEnemyNumber; //敵の数
	int mBrickRate; //煉瓦率(パーセント)
	int mItemPowerNumber; //爆風アイテムの数
	int mItemBombNumber; //爆弾アイテムの数
};

StageData gStageData[] = {
	{ 2, 90, 4, 6, },
	{ 3, 80, 1, 0, },
	{ 6, 30, 0, 1, },
};

} //namespace

State::State( int stageID ) : 
mImage( 0 ),
mStageID( stageID ){
	Framework f = Framework::instance(); //後で何度か使うので
	mStaticObjects.setSize( WIDTH, HEIGHT );

	mImage = new Image( "data/image/bakudanBitoImage.dds" );

	const StageData& stageData = gStageData[ mStageID ];
	int n = HEIGHT * WIDTH; //マス目の数

	//煉瓦のブロックを記録。
	unsigned* brickList = new unsigned[ n ];
	int brickNumber = 0; //本当にレンガになった数をカウント

	for ( int y = 0; y < HEIGHT; ++y ){
		for ( int x = 0; x < WIDTH; ++x ){
			StaticObject& o = mStaticObjects( x, y );
			if ( x == 0 || y == 0 || ( x == WIDTH-1 ) || ( y == HEIGHT-1 ) ){
				o.setFlag( StaticObject::FLAG_WALL );
			}else if ( ( x % 2 == 0 ) && ( y % 2 == 0 ) ){ //コンクリート
				o.setFlag( StaticObject::FLAG_WALL );
			}else if ( y + x < 4 ){
				//左上3マスは床
			}else if ( ( stageID == 0 ) && ( y + x > ( WIDTH + HEIGHT - 6 ) ) ){
				//二人用なら右下3マスも空ける。
			}else{ //残りは煉瓦か床。100面サイコロを振って決める
				if ( f.getRandom( 100 ) < stageData.mBrickRate  ){
					o.setFlag( StaticObject::FLAG_BRICK );
					//煉瓦だったら記録しておく。
					brickList[ brickNumber ] = ( x << 16 ) + y;
					++brickNumber;
				}
			}
		}
	}
	//煉瓦にアイテムを仕込む
	int powerNumber = stageData.mItemPowerNumber;
	int bombNumber = stageData.mItemBombNumber;
	//やり方は、煉瓦リストのi番目を適当なものと取り替えて、そこにアイテムを入れる。
	for ( int i = 0; i < powerNumber + bombNumber; ++i ){
 		int swapped = f.getRandom( brickNumber - 1 - i ) + i; //自分か、自分より後ろと取り替える。でないとすでに入れたマスがもう一回出てきてしまう。
		unsigned t = brickList[ i ];
		brickList[ i ] = brickList[ swapped ];
		brickList[ swapped ] = t;

		int x = brickList[ i ] >> 16;
		int y = brickList[ i ] & 0xffff;
		StaticObject& o = mStaticObjects( x, y );
		if ( i < powerNumber ){
			o.setFlag( StaticObject::FLAG_ITEM_POWER );
		}else{
			o.setFlag( StaticObject::FLAG_ITEM_BOMB );
		}
	}
	SAFE_DELETE_ARRAY( brickList );
}

State::~State(){
	SAFE_DELETE( mImage );
}

void State::draw() const {
	//背景描画
	for ( int y = 0; y < HEIGHT; ++y ){
		for ( int x = 0; x < WIDTH; ++x ){
			mStaticObjects( x, y ).draw( x, y, mImage );
		}
	}
	//TODO:前景描画
	//TODO:爆風描画
}

void State::update(){
	//TODO:
}

bool State::hasCleared() const {
	//TODO:
	return false;
}

bool State::isAlive1P() const {
	//TODO:
	return true;
}

bool State::isAlive2P() const {
	//TODO:
	return true;
}

