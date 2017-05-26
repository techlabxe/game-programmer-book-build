#include "GameLib/Framework.h"
#include "GameLib/Math.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
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
const int gHitListBlockSize = 10000; //てきとう
bool testCircles( int index0, int index1 ); //1個づつの判定関数(注:中で力は与えない)
void addForce( int i0, int i1 ); //力を足すのはこっち
const int MAX_LEVEL = 13; //最大分割段数

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

struct Node{
	enum Direction{
		DIR_X,
		DIR_Y,
		DIR_NONE,
	};
	Node() : 
	mDirection( DIR_NONE ),
	mLeft( 0 ), 
	mRight( 0 ), 
	mIndices( 0 ),
	mIndexNumber ( 0 ){
	}
	//再帰構築(引数はx,yの範囲)
	void build( 
		double x0, 
		double x1,
		double y0,
		double y1,
		int* indices,
		int* indexPos,
		Node* nodes,
		int* nodePos,
		int restLevel, //あと何回分裂するのか？
		bool drawLine );
	//再帰判定
	void detect( list< HitPair* >& hitList, int* hitListBlockPos, int* test, int* hit ) const; 

	Direction mDirection;
	Node* mLeft;
	Node* mRight;
	int* mIndices;
	int mIndexNumber;
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

	//ノード配列を用意。
	//最大MAX_LEVEL段になるので、ノードの数は2^MAX_LEVEL-1
	//1段なら1、2段なら1+2、3段なら1+2+4、と考えていけばいい。
	int nodePosMax = 1;
	for ( int i = 0; i < MAX_LEVEL; ++i ){
		nodePosMax *= 2;
	}
	nodePosMax -= 1;
	//インデクス数の最大値はいくつになるか？
	//前もってはわからないので100倍重複するとしておく。これは過剰。
	int indexPosMax = n * 100;
	Node* nodes = new Node[ nodePosMax ];
	int* indices = new int[ indexPosMax ];
	//第一ノードを用意する。
	Node root;
	root.mIndices = indices;
	for ( int i = 0; i < n; ++i ){
		root.mIndices[ i ] = i;
	}
	root.mIndexNumber = n;
	int indexPos = n; //切り出し位置
	int nodePos = 1; //切り出し位置
	//再帰構築(この関数の中が今回のミソ)
	bool draw = true;
	if ( Input::Manager::instance().keyboard().isOn( ' ' ) ){
		draw = false;
	}
	root.build( 
		gMinimum.x, 
		gMaximum.x, 
		gMinimum.y,
		gMaximum.y, 
		indices, 
		&indexPos, 
		nodes, 
		&nodePos, 
		MAX_LEVEL - 1, //後MAX_LEVEL-1回分割する、の意味。
		draw );
	//再帰判定(この関数の中はそんなに難しくない)
	list< HitPair* > hitList;
	int hitListBlockPos = gHitListBlockSize;
	root.detect( hitList, &hitListBlockPos, test, hit ); //第二引数は更新するのでポインタ

	//重複排除して衝突応答。ここから下はArrayList3と同一。
	//コードをシンプルにしてかつ高速化するため、
	//配列のリストを普通の配列に直す
	HitPair* hitListArray = new HitPair[ hitList.size() * gHitListBlockSize ];
	int hitListArraySize = 0;
	typedef list< HitPair* >::iterator HIt; //別名
	int blockPos = 0;
	int blockNumber = static_cast< int >( hitList.size() );
	for ( HIt i = hitList.begin(); i != hitList.end(); ++i ){
		//現ブロックのサイズを求める。最後だけ数が違う。入れている途中だったからだ。
		int blockSize = ( blockPos == ( blockNumber - 1 ) ) ? hitListBlockPos : gHitListBlockSize;
		for ( int j = 0; j < blockSize; ++j ){
			hitListArray[ hitListArraySize ] = ( *i )[ j ];
			++hitListArraySize;
		}
		SAFE_DELETE_ARRAY( *i ); //もういらないので消去
		++blockPos;
	}

	//まずペアの一番目がiであるものがいくつあるかを数える
	int* hitListSize = new int[ n ];
	//初期化
	for ( int i = 0; i < n; ++i ){
		hitListSize[ i ] = 0;
	}
	//ループで数える
	for ( int i = 0; i < hitListArraySize; ++i ){
		++hitListSize[ hitListArray[ i ].mI0 ];
	}
	//hitListSizeは数だが、これを先頭からオフセットに変換する。
	int* hitListOffset = new int[ n ];
	int offset = 0;
	for ( int i = 0; i < n; ++i ){
		hitListOffset[ i ] = offset;
		offset += hitListSize[ i ];
		hitListSize[ i ] = 0; //サイズ配列は0にしてしまう。
		//次に入れていく時にここに数えなおせばまた同じものが完成する。
		//第三段階でどこまで入れたかを覚えておくための配列を別に作らないための姑息な工夫。
	}
	//物体配列を確保。ちょうど上のoffsetに合計が入っている。
	int* hitArray = new int[ offset ];
	for ( int i = 0; i < hitListArraySize; ++i ){
		const HitPair& o = hitListArray[ i ];
		hitArray[ hitListOffset[ o.mI0 ] + hitListSize[ o.mI0 ] ] = o.mI1;
		++hitListSize[ o.mI0 ];//サイズを+1。このループを抜けた時には最初に数えた時と同じ状態になっている。
	}
	SAFE_DELETE_ARRAY( hitListArray );

	//後は箱ごとに処理。sortもuniqueもしないが、箱一つ一つが小さいのでいらない工夫をするより単純にやった方が速いのだ。
	for ( int i = 0; i < n; ++i ){
		int* box = &hitArray[ hitListOffset[ i ] ];
		int boxSize = hitListSize[ i ];
		for ( int j = 0; j < boxSize; ++j ){
			if ( box[ j ] >= 0 ){
				addForce( i, box[ j ] );
				//今やった奴が後ろにいるようならそいつをつぶす。
				for ( int k = j + 1; k < boxSize; ++k ){
					if ( box[ k ] == box[ j ] ){
						box[ k ] = -1;
					}
				}
			}
		}
	}
	//後始末
	SAFE_DELETE_ARRAY( hitArray );
	SAFE_DELETE_ARRAY( hitListSize );
	SAFE_DELETE_ARRAY( hitListOffset );
	SAFE_DELETE_ARRAY( indices );
	SAFE_DELETE_ARRAY( nodes );
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

//ここが今回のミソですよ
void Node::build( 
double x0, 
double x1, 
double y0, 
double y1,
int* indices,
int* indexPos,
Node* nodes,
int* nodePos,
int restLevel, //後何回割るか
bool draw ){
	//ここでは一番単純な方法で割る。
	//x,yの長い方で割るのだ。
	double div; //分割線
	if ( ( x1 - x0 ) > ( y1 - y0 ) ){
		mDirection = DIR_X;
		div = ( x0 + x1 ) * 0.5;
	}else{
		mDirection = DIR_Y;
		div = ( y0 + y1 ) * 0.5;
	}
	//数える準備
	int c0, c1;
	c0 = c1 = 0; //左右ノードに割り振る三角形の数
	//子ノード確保
	mLeft = &nodes[ *nodePos + 0 ];
	mRight = &nodes[ *nodePos + 1 ];
	*nodePos += 2;
	//後は方向に応じて分岐
	if ( mDirection == DIR_X ){
		for ( int i = 0; i < mIndexNumber; ++i ){
			const Circle& c = gCircles[ mIndices[ i ] ]; //グローバルは嫌だね。マネするんじゃないよ？
			const Vector2& p = c.mPosition;
			if ( p.x - R <= div ){ ++c0; } //ここのコードは数値演算誤差を考えると不適当。考えてみよう。
			if ( p.x + R >= div ){ ++c1; } //正しくは<= divX+e, >= divX-eとなる。eは誤差許容範囲だ。
		}
		//子配列切り出し
		mLeft->mIndices = indices + *indexPos;
		*indexPos += c0;
		mRight->mIndices = indices + *indexPos;
		*indexPos += c1;
		//分配
		for ( int i = 0; i < mIndexNumber; ++i ){
			int idx = mIndices[ i ];
			const Circle& c = gCircles[ idx ]; //グローバルは嫌だね。マネするんじゃないよ？
			const Vector2& p = c.mPosition;
			if ( p.x - R <= div ){
				mLeft->mIndices[ mLeft->mIndexNumber ] = idx;
				++mLeft->mIndexNumber;
			}
			if ( p.x + R >= div ){
				mRight->mIndices[ mRight->mIndexNumber ] = idx;
				++mRight->mIndexNumber;
			}
		}
		if ( restLevel > 1 ){ //子にまだ分割させるなら(>0でない理由を考えてみよう)
			if ( c0 > 1 ){ //2個以上ないと割る意味はない
				mLeft->build( x0, div, y0, y1, indices, indexPos, nodes, nodePos, restLevel - 1, draw );
			}
			if ( c1 > 1 ){
				mRight->build( div, x1, y0, y1, indices, indexPos, nodes, nodePos, restLevel - 1, draw );
			}
		}
		if ( draw ){ //分割線描画。
			double p[ 4 ][ 2 ];
			p[ 0 ][ 0 ] = p[ 1 ][ 0 ] = div - 0.25 + 160.0;
			p[ 2 ][ 0 ] = p[ 3 ][ 0 ] = div + 0.25 + 160.0;
			p[ 0 ][ 1 ] = p[ 2 ][ 1 ] = y0 + 120.0;
			p[ 1 ][ 1 ] = p[ 3 ][ 1 ] = y1 + 120.0;
			Framework::instance().drawTriangle2D( p[ 0 ], p[ 1 ], p[ 2 ], 0, 0, 0, 0xff00ff00, 0xff00ff00, 0xff00ff00 );
			Framework::instance().drawTriangle2D( p[ 3 ], p[ 1 ], p[ 2 ], 0, 0, 0, 0xff00ff00, 0xff00ff00, 0xff00ff00 );
		}
	}else{
		for ( int i = 0; i < mIndexNumber; ++i ){
			const Circle& c = gCircles[ mIndices[ i ] ]; //グローバルは嫌だね。マネするんじゃないよ？
			const Vector2& p = c.mPosition;
			if ( p.y - R <= div ){ ++c0; } //ここのコードは数値演算誤差を考えると不適当。考えてみよう。
			if ( p.y + R >= div ){ ++c1; } //正しくは<= divX+e, >= divX-eとなる。eは誤差許容範囲だ。
		}
		//子配列切り出し
		mLeft->mIndices = indices + *indexPos;
		*indexPos += c0;
		mRight->mIndices = indices + *indexPos;
		*indexPos += c1;
		//分配
		for ( int i = 0; i < mIndexNumber; ++i ){
			int idx = mIndices[ i ];
			const Circle& c = gCircles[ idx ]; //グローバルは嫌だね。マネするんじゃないよ？
			const Vector2& p = c.mPosition;
			if ( p.y - R <= div ){
				mLeft->mIndices[ mLeft->mIndexNumber ] = idx;
				++mLeft->mIndexNumber;
			}
			if ( p.y + R >= div ){
				mRight->mIndices[ mRight->mIndexNumber ] = idx;
				++mRight->mIndexNumber;
			}
		}
		if ( restLevel > 1 ){ //子にまだ分割させるなら(>0でない理由を考えてみよう)
			if ( c0 > 1 ){ //2個以上ないと割る意味はない
				mLeft->build( x0, x1, y0, div, indices, indexPos, nodes, nodePos, restLevel - 1, draw );
			}
			if ( c1 > 1 ){
				mRight->build( x0, x1, div, y1, indices, indexPos, nodes, nodePos, restLevel - 1, draw );
			}
		}
		if ( draw ){ //分割線描画。
			double p[ 4 ][ 2 ];
			p[ 0 ][ 0 ] = p[ 1 ][ 0 ] = x0 + 160.0;
			p[ 2 ][ 0 ] = p[ 3 ][ 0 ] = x1 + 160.0;
			p[ 0 ][ 1 ] = p[ 2 ][ 1 ] = div - 0.25 + 120.0;
			p[ 1 ][ 1 ] = p[ 3 ][ 1 ] = div + 0.25 + 120.0;
			Framework::instance().drawTriangle2D( p[ 0 ], p[ 1 ], p[ 2 ], 0, 0, 0, 0xff00ff00, 0xff00ff00, 0xff00ff00 );
			Framework::instance().drawTriangle2D( p[ 3 ], p[ 1 ], p[ 2 ], 0, 0, 0, 0xff00ff00, 0xff00ff00, 0xff00ff00 );
		}
	}
	//自分の配列は要らない。しかし開放はできないので、間違いが起こらぬよう0にしておく。
	//なお、ここを後の部分で再利用するようにコードを書くことも出来、
	//そうすれば前もって確保する配列をかなり短くできるが、かなり複雑になる。
	mIndices = 0;
	mIndexNumber = 0;
}

//使う方は簡単
void Node::detect( list< HitPair* >& hitList, int* hitListBlockPos, int* test, int* hit ) const{
	if ( mDirection == DIR_NONE ){ //分割がないイコールここが終点。
		for ( int i = 0; i < mIndexNumber; ++i ){
			int i0 = mIndices[ i ];
			for ( int j = i + 1; j < mIndexNumber; ++j ){
				int i1 = mIndices[ j ];
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
					if ( *hitListBlockPos == gHitListBlockSize ){
						hitList.push_back( new HitPair[ gHitListBlockSize ] );
						*hitListBlockPos = 0;
					}
					hitList.back()[ *hitListBlockPos ] = hit;
					++( *hitListBlockPos );
				}
			}
		}
	}else{ //子がいるので子に渡す。
		mLeft->detect( hitList, hitListBlockPos, test, hit );
		mRight->detect( hitList, hitListBlockPos, test, hit );
	}
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
