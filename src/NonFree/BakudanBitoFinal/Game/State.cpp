#include "GameLib/Framework.h"

#include "Game/State.h"
#include "Game/StaticObject.h"
#include "Game/DynamicObject.h"
#include "SoundManager.h"
#include "Image.h"

using namespace GameLib;

namespace {

//マップの広さ
static const int WIDTH = 19;
static const int HEIGHT = 15;
//爆弾パラメータ
static const int EXPLOSION_TIME = 180; //3秒
static const int EXPLOSION_LIFE = 60; //1秒

//適当ステージデータ
struct StageData{
	int mEnemyNumber; //敵の数
	int mBrickRate; //煉瓦率(パーセント)
	int mItemPowerNumber; //爆風アイテムの数
	int mItemBombNumber; //爆弾アイテムの数
};

static StageData gStageData[] = {
	{ 2, 50, 10, 10, },
	{ 3, 70, 1, 0, },
	{ 5, 30, 0, 1, },
};

} //namespace{}

State::State( int stageID ) : 
mImage( 0 ),
mDynamicObjects( 0 ),
mDynamicObjectNumber( 0 ),
mStageID( stageID ){
	Framework f = Framework::instance(); //後で何度か使うので
	mStaticObjects.setSize( WIDTH, HEIGHT );

	mImage = new Image( "data/image/bakudanBitoImage.tga" );

	const StageData& stageData = gStageData[ mStageID ];
	int n = HEIGHT * WIDTH; //マス目の数

	//煉瓦のブロックを記録。
	unsigned* brickList = new unsigned[ n ];
	int brickNumber = 0; //本当にレンガになった数をカウント
	unsigned* floorList = new unsigned[ n ];
	int floorNumber = 0;

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
				}else{
					floorList[ floorNumber ] = ( x << 16 ) + y;
					++floorNumber;
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

	//動的オブジェクトを確保
	int playerNumber = ( mStageID == 0 ) ? 2 : 1;
	int enemyNumber = stageData.mEnemyNumber;
	mDynamicObjectNumber = playerNumber + enemyNumber;
	mDynamicObjects = new DynamicObject[ mDynamicObjectNumber ];

	//プレイヤー配置
	mDynamicObjects[ 0 ].set( 1, 1, DynamicObject::TYPE_PLAYER );
	mDynamicObjects[ 0 ].mPlayerID = 0;
	if ( mStageID == 0 ){
		mDynamicObjects[ 1 ].set( WIDTH-2, HEIGHT-2, DynamicObject::TYPE_PLAYER );
		mDynamicObjects[ 1 ].mPlayerID = 1;
	}
	//床に敵を仕込む。やり方はアイテムとほとんど同じ
	for ( int i = 0; i < enemyNumber; ++i ){
 		int swapped = f.getRandom( floorNumber - 1 - i ) + i;
		unsigned t = floorList[ i ];
		floorList[ i ] = floorList[ swapped ];
		floorList[ swapped ] = t;

		int x = floorList[ i ] >> 16;
		int y = floorList[ i ] & 0xffff;
		mDynamicObjects[ playerNumber + i ].set( x, y, DynamicObject::TYPE_ENEMY );
	}
	SAFE_DELETE_ARRAY( floorList );
}

State::~State(){
	SAFE_DELETE( mImage );
	SAFE_DELETE_ARRAY( mDynamicObjects );
}

void State::draw() const {
	//背景描画
	for ( int y = 0; y < HEIGHT; ++y ){
		for ( int x = 0; x < WIDTH; ++x ){
			mStaticObjects( x, y ).draw( x, y, mImage );
		}
	}
	//前景描画
	for ( int i = 0; i < mDynamicObjectNumber; ++i ){
		mDynamicObjects[ i ].draw( mImage );
	}
	//爆風描画
	for ( int y = 0; y < HEIGHT; ++y ){
		for ( int x = 0; x < WIDTH; ++x ){
			mStaticObjects( x, y ).drawExplosion( x, y, mImage );
		}
	}
}

void State::update(){
	//爆弾の処理
	for ( int y = 0; y < HEIGHT; ++y ){
		for ( int x = 0; x < WIDTH; ++x ){
			StaticObject& o = mStaticObjects( x, y );
			if ( o.checkFlag( StaticObject::FLAG_BOMB ) ){
				//1.爆弾のカウントを更新
				++o.mCount;
				//2.爆破開始、終了判定
				if ( o.checkFlag( StaticObject::FLAG_EXPLODING ) ){ //消火判定
					if ( o.mCount == EXPLOSION_LIFE ){ //爆発終了時刻になった
						o.resetFlag( StaticObject::FLAG_EXPLODING | StaticObject::FLAG_BOMB );
						o.mCount = 0;
					}
				}else{ //爆破判定
					if ( o.mCount == EXPLOSION_TIME ){ //爆破時刻になった
						o.setFlag( StaticObject::FLAG_EXPLODING );
						o.mCount = 0;
						SoundManager::instance()->playSe( SoundManager::SE_EXPLOSION );
					}else if ( o.checkFlag( StaticObject::FLAG_FIRE_X | StaticObject::FLAG_FIRE_Y ) ){ //誘爆
						o.setFlag( StaticObject::FLAG_EXPLODING );
						o.mCount = 0;
						SoundManager::instance()->playSe( SoundManager::SE_EXPLOSION );
					}
				}
			}else if ( o.checkFlag( StaticObject::FLAG_BRICK ) ){ //レンガの場合焼け落ち判定が必要
				if ( o.checkFlag( StaticObject::FLAG_FIRE_X | StaticObject::FLAG_FIRE_Y ) ){ //火がついている
					++o.mCount; //前のフレームでついた火なので判定前にインクリメント
					if ( o.mCount == EXPLOSION_LIFE ){
						o.mCount = 0;
						o.resetFlag( StaticObject::FLAG_BRICK ); //焼け落ちた
					}
				}
			}
			//3.爆風は毎フレーム置きなおすので、一回消す。
			o.resetFlag( StaticObject::FLAG_FIRE_X | StaticObject::FLAG_FIRE_Y );
		}
	}
	//炎設置
	for ( int y = 0; y < HEIGHT; ++y ){
		for ( int x = 0; x < WIDTH; ++x ){
			if ( mStaticObjects( x, y ).checkFlag( StaticObject::FLAG_EXPLODING ) ){
				setFire( x, y );
			}
		}
	}

	//1P,2Pの設置爆弾数をカウント
	int bombNumber[ 2 ];
	bombNumber[ 0 ] = bombNumber[ 1 ] = 0;
	for ( int y = 0; y < HEIGHT; ++y ){
		for ( int x = 0; x < WIDTH; ++x ){
			const StaticObject& o = mStaticObjects( x, y );
			if ( o.checkFlag( StaticObject::FLAG_BOMB ) ){
				++bombNumber[ o.mBombOwner->mPlayerID ];
			}
		}
	}
	//ダイナミックオブジェクトでループ
	for ( int i = 0; i < mDynamicObjectNumber; ++i ){
		DynamicObject& o = mDynamicObjects[ i ];
		if ( o.isDead() ){ //死んでる。終わる。
			continue;
		}
		//置いた爆弾と接触しているかチェック
		for ( int j = 0; j < 2; ++j ){
			if ( o.mLastBombX[ j ] >= 0 ){ //0以上なら何か入っている。
				if ( !o.isIntersectWall( o.mLastBombX[ j ], o.mLastBombY[ j ] ) ){
					o.mLastBombX[ j ] = o.mLastBombY[ j ] = -1;
				}
			}
		}
		//現在セルを取得
		int x, y;
		o.getCell( &x, &y );
		//これを中心とするセルの中から壁を探して配列に格納
		int wallsX[ 9 ];
		int wallsY[ 9 ];
		int wallNumber = 0;
		for ( int i = 0; i < 3; ++i ){
			for ( int j = 0; j < 3; ++j ){
				int tx = x + i - 1;
				int ty = y + j - 1;
				const StaticObject& so = mStaticObjects( tx, ty );
				if ( so.checkFlag( StaticObject::FLAG_WALL | StaticObject::FLAG_BRICK | StaticObject::FLAG_BOMB ) ){ //壁で
					bool myBomb0 = ( o.mLastBombX[ 0 ] == tx ) && ( o.mLastBombY[ 0 ] == ty );
					bool myBomb1 = ( o.mLastBombX[ 1 ] == tx ) && ( o.mLastBombY[ 1 ] == ty );
					if ( !myBomb0 && !myBomb1 ){ //自分が置いた爆弾じゃない
						wallsX[ wallNumber ] = x + i - 1;
						wallsY[ wallNumber ] = y + j - 1;
						++wallNumber;
					}
				}
			}
		}
		//壁リストを渡して移動処理
		o.move( wallsX, wallsY, wallNumber );
		//移動後の位置で周囲9マスと衝突判定していろいろな反応
		for ( int i = 0; i < 3; ++i ){
			for ( int j = 0; j < 3; ++j ){
				StaticObject& so = mStaticObjects( x + i - 1, y + j - 1 );
				if ( o.isIntersectWall( x + i - 1, y + j - 1 ) ){ //触ってます
					if ( so.checkFlag( StaticObject::FLAG_FIRE_X | StaticObject::FLAG_FIRE_Y ) ){
						o.die(); //焼かれた
					}else if ( !so.checkFlag( StaticObject::FLAG_BRICK ) ){ //あらわになっているアイテムがあれば
						if ( so.checkFlag( StaticObject::FLAG_ITEM_POWER ) ){
							so.resetFlag( StaticObject::FLAG_ITEM_POWER );
							++o.mBombPower;
						}else if ( so.checkFlag( StaticObject::FLAG_ITEM_BOMB ) ){
							so.resetFlag( StaticObject::FLAG_ITEM_BOMB );
							++o.mBombNumber;
						}
					}
				}
			}
		}
		//移動後セル番号を取得
		o.getCell( &x, &y );
		//爆弾を置く
		if ( o.hasBombButtonPressed() ){ //爆弾設置ボタンが押されていて
			if ( bombNumber[ o.mPlayerID ] < o.mBombNumber ){ //爆弾最大値未満で
				StaticObject& so = mStaticObjects( x, y );
				if ( !so.checkFlag( StaticObject::FLAG_BOMB ) ){ //爆弾がない
					so.setFlag( StaticObject::FLAG_BOMB );
					so.mBombOwner = &o;
					so.mCount = 0;

					//置いた爆弾位置を更新
					if ( o.mLastBombX[ 0 ] < 0 ){
						o.mLastBombX[ 0 ] = x;
						o.mLastBombY[ 0 ] = y;
					}else{
						o.mLastBombX[ 1 ] = x;
						o.mLastBombY[ 1 ] = y;
					}
					SoundManager::instance()->playSe( SoundManager::SE_SET_BOMB );
				}
			}
		}
	}
	//次。敵とプレイヤーの接触判定。
	for ( int i = 0; i < mDynamicObjectNumber; ++i ){
		for ( int j = i + 1; j < mDynamicObjectNumber; ++j ){
			mDynamicObjects[ i ].doCollisionReactionToDynamic( &mDynamicObjects[ j ] );
		}
	}
}

void State::setFire( int x, int y ){
	StaticObject& o = mStaticObjects( x, y );
	int power = o.mBombOwner->mBombPower;
	int end;
	
	//ループ内の処理はほとんど4ループで一緒。
	//やろうと思えば共通化はできる。しかし、それによって何をやるのかわかりにくい関数ができてしまう欠点もある。
	//ここでは4回同じものを書いた。

	//左
	end = ( x - power < 0 ) ? 0 : ( x - power );
	for ( int i = x - 1; i >= end; --i ){
		StaticObject& to = mStaticObjects( i, y );
		to.setFlag( StaticObject::FLAG_FIRE_X );
		if ( to.checkFlag( StaticObject::FLAG_WALL | StaticObject::FLAG_BRICK | StaticObject::FLAG_BOMB ) ){ //何か置いてあれば火は止まる
			break;
		}else{
			//もしアイテムがあれば抹殺
			to.resetFlag( StaticObject::FLAG_ITEM_BOMB | StaticObject::FLAG_ITEM_POWER );
		}
	}
	//右
	end = ( x + power >= WIDTH ) ? ( WIDTH - 1 ) : ( x + power );
	for ( int i = x + 1; i <= end; ++i ){
		StaticObject& to = mStaticObjects( i, y );
		to.setFlag( StaticObject::FLAG_FIRE_X );
		if ( to.checkFlag( StaticObject::FLAG_WALL | StaticObject::FLAG_BRICK | StaticObject::FLAG_BOMB ) ){ //何か置いてあれば火は止まる
			break;
		}else{
			//もしアイテムがあれば抹殺
			to.resetFlag( StaticObject::FLAG_ITEM_BOMB | StaticObject::FLAG_ITEM_POWER );
		}
	}
	//上
	end = ( y - power < 0 ) ? 0 : ( y - power );
	for ( int i = y - 1; i >= end; --i ){
		StaticObject& to = mStaticObjects( x, i );
		to.setFlag( StaticObject::FLAG_FIRE_Y );
		if ( to.checkFlag( StaticObject::FLAG_WALL | StaticObject::FLAG_BRICK | StaticObject::FLAG_BOMB ) ){ //何か置いてあれば火は止まる
			break;
		}else{
			//もしアイテムがあれば抹殺
			to.resetFlag( StaticObject::FLAG_ITEM_BOMB | StaticObject::FLAG_ITEM_POWER );
		}
	}
	//下
	end = ( y + power >= HEIGHT ) ? ( HEIGHT - 1 ) : ( y + power );
	for ( int i = y + 1; i <= end; ++i ){
		StaticObject& to = mStaticObjects( x, i );
		to.setFlag( StaticObject::FLAG_FIRE_Y );
		if ( to.checkFlag( StaticObject::FLAG_WALL | StaticObject::FLAG_BRICK | StaticObject::FLAG_BOMB ) ){ //何か置いてあれば火は止まる
			break;
		}else{
			//もしアイテムがあれば抹殺
			to.resetFlag( StaticObject::FLAG_ITEM_BOMB | StaticObject::FLAG_ITEM_POWER );
		}
	}

	//[ここから下は理解が困難]

	//ここから下は
	//連鎖爆発のタイミングズレのために煉瓦が焼け落ちるタイミングがズレてしまう問題を解決するためにある。
	//○○□□
	//とあって○が爆弾、□が煉瓦とする。右から順に爆発すると、まず
	//○●■□
	//となる。●が爆発中、■が焼けた煉瓦。その後連鎖していって、いずれ煉瓦が焼け落ち、
	//●　　□
	//と中央二個が消える。すると、左の爆風を遮るものがなくなって、
	//●→→■
	//と焼かれてしまうのである。これを防ぐには、煉瓦は連鎖する爆弾の中で最も遅いものに合わせて焼け落ちねばならない。
	//そのために、爆発したての爆風が届く範囲の煉瓦のカウントを0に初期化してやる必要があるのである。
	//いろいろなやり方があるが、半ばコピペで済むこのやり方を採用してみた。
	//でも、本当は爆発開始時に爆風の及ぶマスを固定する方法の方が正しい。
	//爆弾クラスを別個用意して、自分が焼くマスのリストを持つのが素直だろう。
	//しかしそれは大改造になってしまうので、サンプルとしてはよろしくないと思い、こうしてある。

	//左
	end = ( x - power < 0 ) ? 0 : ( x - power );
	for ( int i = x - 1; i >= end; --i ){
		StaticObject& to = mStaticObjects( i, y );
		if ( to.checkFlag( StaticObject::FLAG_WALL | StaticObject::FLAG_BRICK ) ){ //爆弾は素通りする。どうせ連鎖して消えるからだ。
			if ( ( o.mCount == 0 ) && to.checkFlag( StaticObject::FLAG_BRICK ) ){ //レンガなら焼け落ちカウント開始
				to.mCount = 0;
			}
			break;
		}
	}
	//右
	end = ( x + power >= WIDTH ) ? ( WIDTH - 1 ) : ( x + power );
	for ( int i = x + 1; i <= end; ++i ){
		StaticObject& to = mStaticObjects( i, y );
		if ( to.checkFlag( StaticObject::FLAG_WALL | StaticObject::FLAG_BRICK ) ){ 
			if ( ( o.mCount == 0 ) && to.checkFlag( StaticObject::FLAG_BRICK ) ){ //レンガなら焼け落ちカウント開始
				to.mCount = 0;
			}
			break;
		}
	}
	//上
	end = ( y - power < 0 ) ? 0 : ( y - power );
	for ( int i = y - 1; i >= end; --i ){
		StaticObject& to = mStaticObjects( x, i );
		if ( to.checkFlag( StaticObject::FLAG_WALL | StaticObject::FLAG_BRICK ) ){ 
			if ( ( o.mCount == 0 ) && to.checkFlag( StaticObject::FLAG_BRICK ) ){ //レンガなら焼け落ちカウント開始
				to.mCount = 0;
			}
			break;
		}
	}
	//下
	end = ( y + power >= HEIGHT ) ? ( HEIGHT - 1 ) : ( y + power );
	for ( int i = y + 1; i <= end; ++i ){
		StaticObject& to = mStaticObjects( x, i );
		if ( to.checkFlag( StaticObject::FLAG_WALL | StaticObject::FLAG_BRICK ) ){
			if ( ( o.mCount == 0 ) && to.checkFlag( StaticObject::FLAG_BRICK ) ){ //レンガなら焼け落ちカウント開始
				to.mCount = 0;
			}
			break;
		}
	}
}

bool State::hasCleared() const {
	//敵が残っていなければクリア
	for ( int i = 0; i < mDynamicObjectNumber; ++i ){
		if ( mDynamicObjects[ i ].isEnemy() ){
			return false;
		}
	}
	return true;
}

bool State::isAlive( int playerID ) const {
	//いれば生きている
	for ( int i = 0; i < mDynamicObjectNumber; ++i ){
		if ( mDynamicObjects[ i ].mType == DynamicObject::TYPE_PLAYER ){
			if ( mDynamicObjects[ i ].mPlayerID == playerID ){
				return true;
			}
		}
	}
	return false;
}

