#include "GameLib/Framework.h"
#include "GameLib/GameLibMath.h"
#include "Vector2.h"
#include "List.h"
#include <sstream>
#include <algorithm>
using namespace std;
using namespace GameLib;

struct Circle{
	Vector2 mPosition;
	Vector2 mVelocity;
};
Circle* gCircles = 0;
const int N = 40; //これの二乗個でやる
const double R = 2.0; //半径2ね
const double RSUM2 = ( R + R ) * ( R + R ); //半径和の二乗
bool testCircles( int index0, int index1 ); //1個づつの判定関数

//----------------------ここがこの章のミソ----------------------------------------

//ソートするための構造体
struct Key{
	//キー同士の大小関係を定義
	bool operator<( const Key& a ){
		if ( mX < a.mX ){ //小さければtrue
			return true;
		}else if ( mX > a.mX ){ //大きければfalse
			return false;
		}else if ( mIsLeft && !a.mIsLeft ){ //同じだ。こういう時は左を優先
			return true;
		}else if ( !mIsLeft && a.mIsLeft ){
			return false;
		}else{ //ここまで同じならindexで決めてやる
			return ( mIndex < a.mIndex ); 
		}
	}

	double mX; //X座標
	int mIndex; //円の番号
	bool mIsLeft; //trueなら左端、falseなら右端
};

void processCollision( int* test, int* hit ){
	*test = 0;
	*hit = 0;
#if 1
	int n = N*N; //個数ね
	//ソートするための配列を作る。
	Key* keys = new Key[ n*2 ];
	//キーの中身を埋めます
	for ( int i = 0; i < n; ++ i){
		const Circle& c = gCircles[ i ];
		keys[ 2 * i + 0 ].mX = c.mPosition.x - R; //左端
		keys[ 2 * i + 1 ].mX = c.mPosition.x + R; //右端
		keys[ 2 * i + 0 ].mIndex = keys[ 2 * i + 1 ].mIndex = i; //自分の番号
		keys[ 2 * i + 0 ].mIsLeft = true; //左
		keys[ 2 * i + 1 ].mIsLeft = false; //右
	}
	//ソート
	sort( keys, keys + n*2 );

	List metList( n );
	int* iteratorList = new int[ n ]; //metListのどこに入れたか覚えておく。

	//さあ、左からなめていこう
	for ( int i = 0; i < n*2; ++i ){
		int i0 = keys[ i ].mIndex;
		if ( keys[ i ].mIsLeft ){ //左が来た。
			int iterator = metList.first(); //リスト全部と判定するよ
			while ( !metList.isEnd( iterator ) ){
				int i1 = metList.value( iterator );
				++( *test );
				if ( testCircles( i0, i1 ) ){
					++( *hit );
				}
				iterator = metList.next( iterator );
			}
			//リストに追加
			int pos = metList.addTail( i0 );
			//追加した場所を覚えておこう
			iteratorList[ i0 ] = pos;
		}else{ //右が来た場合。
			//リストから削除しよう。
			//場所を覚えておいたので検索はいらない
			metList.remove( iteratorList[ i0 ] );
		}
	}
	SAFE_DELETE_ARRAY( iteratorList );
	SAFE_DELETE_ARRAY( keys );
#else //デバグ用総当り版
	for ( int i = 0; i < N*N; ++i ){
		for ( int j = i + 1; j < N*N; ++j ){
			++( *test );
			if ( testCircles( i, j ) ){
				++( *hit );
			}
		}
	}
#endif
}

//----------------------ここから下はこの章の本筋ではないコード---------------------------

//2個のcircleを処理する中身。当たるとtrue
bool testCircles( int i0, int i1 ){
	Circle& c0 = gCircles[ i0 ];
	const Vector2& p0 = c0.mPosition;
	Circle& c1 = gCircles[ i1 ];
	const Vector2& p1 = c1.mPosition;
	//距離は？
	Vector2 t;
	t.setSub( p1, p0 );
	double sql = t.squareLength();
	if ( sql < RSUM2 ){
		double l = GameLib::sqrt( sql ) + 0.0000001; //0で割らないように保険
		t *= 0.25 / l; //適当に長さを調整
		//はじき返す。tはp0->p1のベクタだから、これをc1に足し、c0から引く。
		c1.mVelocity += t;
		c0.mVelocity -= t;
		return true;
	}else{
		return false;
	}
}

namespace GameLib{
	void Framework::update(){
		if ( !gCircles ){
			gCircles = new Circle[ N*N ];
			//初期配置x,yについて[-50,50]
			for ( int i = 0; i < N*N; ++i ){
				gCircles[ i ].mPosition.set( 
					static_cast< double >( ( ( i % N ) - N/2 ) * 4 ) + 0.001 * i, //ちょっとずらす
					static_cast< double >( ( ( i / N ) - N/2 ) * 4 ) );
			}
		}
		//速度初期化
		for ( int i = 0;i < N*N; ++i ){
			//速度を原点方向で初期化
			gCircles[ i ].mVelocity.setMul( gCircles[ i ].mPosition, -0.001 );
		}
		int test;
		int hit;
		processCollision( &test, &hit ); //衝突検出関数

		//更新
		for ( int i = 0;i < N*N; ++i ){
			gCircles[ i ].mPosition += gCircles[ i ].mVelocity;

			//描画
			double p[ 4 ][ 2 ];
			p[ 0 ][ 0 ] = p[ 1 ][ 0 ] = gCircles[ i ].mPosition.x - 0.5 + 160.0;
			p[ 2 ][ 0 ] = p[ 3 ][ 0 ] = gCircles[ i ].mPosition.x + 0.5 + 160.0;
			p[ 0 ][ 1 ] = p[ 2 ][ 1 ] = gCircles[ i ].mPosition.y - 0.5 + 120.0;
			p[ 1 ][ 1 ] = p[ 3 ][ 1 ] = gCircles[ i ].mPosition.y + 0.5 + 120.0;
			drawTriangle2D( p[ 0 ], p[ 1 ], p[ 2 ] );
			drawTriangle2D( p[ 3 ], p[ 1 ], p[ 2 ] );
		}
		ostringstream oss;
		oss << frameRate() << " " << test << " " << hit;
		drawDebugString( 0, 0, oss.str().c_str() );
	}
}
