#include "GameLib/Framework.h"
#include "GameLib/Math.h"
#include "Vector2.h"
#include <sstream>
#include <list>
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
const Vector2 gMinimum( -160.0, -160.0 );
const Vector2 gMaximum( 160.0, 160.0 );
const int gDivision = 30;
const int gBoxListSize = 300; //てきとう
const int gHitListSize = 6000; //てきとう
bool testCircles( int index0, int index1 ); //1個づつの判定関数(注:中で力は与えない)
void addForce( int i0, int i1 ); //力を足すのはこっち

//----------------------ここがこの章のミソ--------------Z--------------------------

//当たったペア
struct HitPair{ 
	bool operator<( const HitPair& a ) const { //ソートに必要な不等号
		if ( mI0 < a.mI0 ){
			return true;
		}else if ( mI0 > a.mI0 ){
			return false;
		}else{
			return ( mI1 < a.mI1 );
		}
	}
	bool operator==( const HitPair& a ) const {//uniqueに必要な統合
		return ( ( mI0 == a.mI0 ) && ( mI1 == a.mI1 ) );
	}
	int mI0;
	int mI1;
};

void processCollision( int* test, int* hit ){
	*test = 0;
	*hit = 0;
#if 1
	int n = N*N; //個数ね
	//箱のサイズを計算
	Vector2 boxSize; //箱サイズ
	boxSize.setSub( gMaximum, gMinimum );
	boxSize *= 1.0 / static_cast< float >( gDivision );
	//リスト用意
	int* boxList = new int[ gDivision * gDivision * gBoxListSize ];
	int* boxListPos = new int[ gDivision * gDivision ];
	for ( int i = 0; i < gDivision * gDivision; ++i ){ //リスト位置を初期化
		boxListPos[ i ] = 0;
	}
	for ( int i = 0; i < n; ++i ){
		Vector2 t;
		t.setSub( gCircles[ i ].mPosition, gMinimum );
		double minX = t.x - R;
		double maxX = t.x + R;
		double minY = t.y - R;
		double maxY = t.y + R;
		//箱番号に変換(除算は高速化できるね？)
		int minXBox = static_cast< int >( minX / boxSize.x );
		int maxXBox = static_cast< int >( maxX / boxSize.x );
		int minYBox = static_cast< int >( minY / boxSize.y );
		int maxYBox = static_cast< int >( maxY / boxSize.y );
		ASSERT( minXBox >= 0 && maxXBox < gDivision && minYBox >=0 && maxYBox < gDivision );
		for ( int j = minXBox; j <= maxXBox; ++j ){
			for ( int k = minYBox; k <= maxYBox; ++k ){
				int boxIndex = k * gDivision + j; //箱番号は「Y * (分割数) + X」
				ASSERT( boxListPos[ boxIndex ] < gBoxListSize );
				boxList[ boxIndex * gBoxListSize + boxListPos[ boxIndex ] ] = i;
				++boxListPos[ boxIndex ];
			}
		}
	}
	//箱ごとに総当り
	HitPair* hitArray = new HitPair[ gHitListSize ];
	int hitArrayPos = 0;
	for ( int i = 0; i < gDivision * gDivision; ++i ){
		for ( int j = 0; j < boxListPos[ i ]; ++j ){
			for ( int k = j + 1; k < boxListPos[ i ]; ++k ){
				int i0 = boxList[ i * gBoxListSize + j ];
				int i1 = boxList[ i * gBoxListSize + k ];
++( *test );
				if ( testCircles( i0, i1 ) ){
++( *hit );
					HitPair hit;
					//同じペアは同じでないと困るので、小さい方を前にする
					if ( i0 < i1 ){
						hit.mI0 = i0;
						hit.mI1 = i1;
					}else{
						hit.mI0 = i0;
						hit.mI1 = i1;
					}
					hitArray[ hitArrayPos ] = hit;
					++hitArrayPos;
				}
			}
		}
	}
	//ソート
	sort( hitArray, hitArray + hitArrayPos );
	//重複を除く
	size_t hitN = unique( hitArray, hitArray + hitArrayPos ) - hitArray;
	//力をかける
	for ( size_t i = 0; i < hitN; ++i ){
		addForce( hitArray[ i ].mI0, hitArray[ i ].mI1 );
	}
	SAFE_DELETE_ARRAY( hitArray );
	SAFE_DELETE_ARRAY( boxListPos );
	SAFE_DELETE_ARRAY( boxList );
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
		return true;
	}else{
		return false;
	}
}

void addForce( int i0, int i1 ){
	Vector2 t;
	t.setSub( gCircles[ i0 ].mPosition, gCircles[ i1 ].mPosition );
	double l = t.length();
	t *= 0.25 / l; //適当に長さを調整
	//はじき返す。tはp0->p1のベクタだから、これをc1に足し、c0から引く。
	gCircles[ i0 ].mVelocity += t;
	gCircles[ i1 ].mVelocity -= t;
}

namespace GameLib{
	void Framework::update(){
		if ( !gCircles ){
			gCircles = new Circle[ N*N ];
			//初期配置
			for ( int i = 0; i < N*N; ++i ){
				gCircles[ i ].mPosition.set( 
					static_cast< double >( ( ( i % N ) - N/2 ) * 4 ) + 0.001 * i, //ちょっとずらす
					static_cast< double >( ( ( i / N ) - N/2 ) * 4 ) );
				gCircles[ i ].mVelocity.set( 0.0, 0.0 );
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
