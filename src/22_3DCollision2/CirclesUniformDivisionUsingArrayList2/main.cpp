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
const double R = 2.0; //半径4ね
const double RSUM2 = ( R + R ) * ( R + R ); //半径和の二乗
const Vector2 gMinimum( -160.0, -160.0 );
const Vector2 gMaximum( 160.0, 160.0 );
const int gDivision = 30;
const int gBoxListBlockSize = 1000; //てきとう
const int gHitListBlockSize = 1000; //てきとう
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

//箱番号と円番号のペア
struct ObjPair{
	void set( int box, int circle ){
		mBox = box;
		mCircle = circle;
	}
	int mBox;
	int mCircle;
};

void processCollision( int* test, int* hit ){
	*test = 0;
	*hit = 0;
#if 1
	int n = N*N; //個数ね
	//箱のサイズを計算
	Vector2 boxSize; //箱サイズ
	boxSize.setSub( gMaximum, gMinimum );
	boxSize *= 1.f / static_cast< float >( gDivision );
	//リスト用意
	list< ObjPair* > boxList;
	int boxListBlockPos = gBoxListBlockSize;

	//第一段階。フラットなArrayListに箱番号と物体番号を格納
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
				if ( boxListBlockPos == gBoxListBlockSize ){
					boxList.push_back( new ObjPair[ gBoxListBlockSize ] );
					boxListBlockPos = 0;
				}
				boxList.back()[ boxListBlockPos ].set( boxIndex, i ) ;
				++boxListBlockPos;
			}
		}
	}
	//第二段階。数を数える
	int* boxListSize = new int[ gDivision * gDivision ];
	//初期化
	for ( int i = 0; i < gDivision * gDivision; ++i ){
		boxListSize[ i ] = 0;
	}
	//ループで数える
	typedef list< ObjPair* >::const_iterator It; //別名
	int blockPos = 0;
	int blockNumber = static_cast< int >( boxList.size() );
	for ( It i = boxList.begin(); i != boxList.end(); ++i ){
		//現ブロックのサイズを求める。最後だけ数が違う。入れている途中だったからだ。
		int blockSize = ( blockPos == ( blockNumber - 1 ) ) ? boxListBlockPos : gBoxListBlockSize;
		for ( int j = 0; j < blockSize; ++j ){
			const ObjPair& o = ( *i )[ j ];
			++boxListSize[ o.mBox ];
		}
		++blockPos;
	}
	//boxListSizeは数だが、これを先頭からオフセットに変換する。
	int* boxListOffset = new int[ gDivision * gDivision ];
	int offset = 0;
	for ( int i = 0; i < gDivision * gDivision; ++i ){
		boxListOffset[ i ] = offset;
		offset += boxListSize[ i ];
		boxListSize[ i ] = 0; //サイズ配列は0にしてしまう。
		//次に入れていく時にここに数えなおせばまた同じものが完成する。
		//第三段階でどこまで入れたかを覚えておくための配列を別に作らないための姑息な工夫。
	}
	//第三段階。実際に箱ごとのリストを生成。

	//物体配列を確保。ちょうど上のoffsetに合計が入っている。
	int* boxArray = new int[ offset ];
	//入れていく。
	blockPos = 0;
	blockNumber = static_cast< int >( boxList.size() );
	for ( It i = boxList.begin(); i != boxList.end(); ++i ){
		//現ブロックのサイズを求める。最後だけ数が違う。入れている途中だったからだ。
		int blockSize = ( blockPos == ( blockNumber - 1 ) ) ? boxListBlockPos : gBoxListBlockSize;
		for ( int j = 0; j < blockSize; ++j ){
			const ObjPair& o = ( *i )[ j ];
			boxArray[ boxListOffset[ o.mBox ] + boxListSize[ o.mBox ] ] = o.mCircle;
			++boxListSize[ o.mBox ]; //サイズを+1。このループを抜けた時には最初に数えた時と同じ状態になっている。
		}
		++blockPos;
	}
	//箱ごとに総当り。配列にしてしまった関係上非常に単純化している。
	list< HitPair* > hitList;
	int hitListBlockPos = gHitListBlockSize;
	for ( int i = 0; i < gDivision * gDivision; ++i ){ //箱でループ
		//箱の中の数を準備
		int boxSize = boxListSize[ i ];
		const int* box = &boxArray[ boxListOffset[ i ] ]; //別名
		for ( int j = 0; j < boxSize; ++j ){ //一個目の物体ループ
			for ( int k = j + 1; k < boxSize; ++k ){ //二個目の物体ループ
				int i0 = box[ j ];
				int i1 = box[ k ];
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
					if ( hitListBlockPos == gHitListBlockSize ){
						hitList.push_back( new HitPair[ gHitListBlockSize ] );
						hitListBlockPos = 0;
					}
					hitList.back()[ hitListBlockPos ] = hit;
					++hitListBlockPos;
				}
			}
		}
	}
	//当たったリストを配列にコピー。ソートしたいからだ。
	blockNumber = static_cast< int >( hitList.size() );
	HitPair* hitArray = 0;
	if ( blockNumber > 0 ){
		int hitArraySize = ( blockNumber - 1 ) * gHitListBlockSize + hitListBlockPos;
		hitArray = new HitPair[ hitArraySize ];
		int blockCopied = 0;
		list< HitPair* >::iterator hitListIt = hitList.begin();
		while ( blockCopied < ( blockNumber - 1 ) ){
			for ( int i = 0; i < gHitListBlockSize; ++i ){
				hitArray[ blockCopied * gHitListBlockSize + i ] = ( *hitListIt )[ i ];
			}
			++blockCopied;
			++hitListIt;
		}
		//最終ブロック
		for ( int i = 0; i < hitListBlockPos; ++i ){
			hitArray[ blockCopied * gHitListBlockSize + i ] = hitList.back()[ i ];
		}
		//ソート
		sort( hitArray, hitArray + hitArraySize );
		//重複を除く
		size_t hitN = unique( hitArray, hitArray + hitArraySize ) - hitArray;
		//力をかける
		for ( size_t i = 0; i < hitN; ++i ){
			addForce( hitArray[ i ].mI0, hitArray[ i ].mI1 );
		}
		//後始末
		for ( list< HitPair* >::iterator i = hitList.begin(); i != hitList.end(); ++i ){
			SAFE_DELETE_ARRAY( *i );
		}
		SAFE_DELETE_ARRAY( hitArray );
	}
	//後始末
	SAFE_DELETE_ARRAY( boxArray );
	SAFE_DELETE_ARRAY( boxListSize );
	SAFE_DELETE_ARRAY( boxListOffset );
	for ( list< ObjPair* >::iterator i = boxList.begin(); i != boxList.end(); ++i ){
		SAFE_DELETE_ARRAY( *i );
	}


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
	if ( sql < RSUM2 ){ //半径は4でいいだろ
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
