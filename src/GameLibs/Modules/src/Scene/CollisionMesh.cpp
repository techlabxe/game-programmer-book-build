#include "GameLib/GameLib.h"
#include "GameLib/Scene/CollisionMesh.h"
#include "GameLib/PseudoXml/Document.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
#include "GameLib/Base/RefString.h"
#include "GameLib/Base/Impl/ReferenceType.h"
#include "GameLib/Base/Pool.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Functions.h"
//using namespace std;
using namespace GameLib;
using namespace GameLib::PseudoXml;
using namespace GameLib::Math;

//#define STRONG_DEBUG //この行が生きているとk-d treeと総当りの結果が完全に一致するかを毎回確認する。
int G;

namespace{

//連立方程式を解く関数。クラーメル法にて解く。
//| ax bx cx | | ox | = | dx |
//| ay by cy | | oy | = | dy |
//| az bz cz | | oz | = | dz |
//という形で3次元ベクタ4つを受け取り、1つに解を入れて返す。oが求める解である。
//ただし、効率化のために、解が一つできる度に判定を行って、範囲外なら即座にfalseを返す
//trueが返るのは当たった時だけである。
bool solveLinearSystem(
Vector3* o,
const Vector3& a,
const Vector3& b,
const Vector3& c,
const Vector3& d ){
	float m1021 = a.y * b.z;
	float m1022 = a.y * c.z;

	float m1120 = b.y * a.z;
	float m1122 = b.y * c.z;

	float m1220 = c.y * a.z;
	float m1221 = c.y * b.z;

	//2項積の差を三つ。行列式用だが、後で使いまわせる。
	float m1122_m1221 = m1122 - m1221;
	float m1220_m1022 = m1220 - m1022; //マイナス
	float m1021_m1120 = m1021 - m1120;

	//行列式
	//00*( 11*22 - 12*21 ) - 01*( 10*22 - 12*20 ) + 02*( 10*21 - 11*20 )
	float delta = a.x*( m1122_m1221 ) + b.x*( m1220_m1022 ) + c.x*( m1021_m1120 );
	if ( delta == 0.f ){ //解けない。ここでは演算誤差は無視し、厳密に0になった時のみはじく。
		return false;
	}
	float rcpDelta = 1.f / delta;	
	
	//演算誤差の問題は極めて面倒である。
	//ここでは桁落ちが3桁にとどまると考え、EPSILON*1000を使う。
	//桁落ちで3桁落ちるには内積が0.001以下になる必要があり、それは0.05度以下の平行だけだ。
	static const float zero = 0.f - EPSILON * 1000.f;
	static const float one = 1.f + EPSILON * 1000.f;

	//できた逆行列に右辺ベクタを乗じつつ解を計算する。deltaの除算は最後に行う。
	float m0221 = c.x * b.z;
	float m0122 = b.x * c.z;
	float m0112 = b.x * c.y;
	float m0211 = c.x * b.y;
	o->x = m1122_m1221 * d.x + ( m0221-m0122 ) * d.y + ( m0112-m0211 ) * d.z;
	//tの判定を行う。
	o->x *= rcpDelta;
	if ( ( o->x < zero ) || ( o->x > one ) ){
		return false;
	}
	//次はuを計算
	float m0022 = a.x * c.z;
	float m0220 = c.x * a.z;
	float m0210 = c.x * a.y;
	float m0012 = a.x * c.y;
	o->y = m1220_m1022 * d.x + ( m0022-m0220 ) * d.y + ( m0210-m0012 ) * d.z;
	o->y *= rcpDelta;
	if ( o->y < zero ){
		return false;
	}
	//最後にvを計算
	float m0120 = b.x * a.z;
	float m0021 = a.x * b.z;
	float m0011 = a.x * b.y;
	float m0110 = b.x * a.y;
	o->z = m1021_m1120 * d.x + ( m0120-m0021 ) * d.y + ( m0011-m0110 ) * d.z;
	o->z *= rcpDelta;
	if ( o->z < zero ){
		return false;
	}
	//u,vの和が1以下かチェック
	if ( ( o->y + o->z ) > one ){
		return false;
	}
	return true;
}

struct Triangle{
	int mIndices[ 3 ];
};

//衝突処理をお願いする時に渡すもの。引数を減らしてオーバーヘッドを減らすための苦肉の策
struct Query{
	Query( 
	const Vector3& begin,
	const Vector3& vec,
	const Triangle* triangles,
	const Vector3* vertices ) :
	mBegin( begin ), mVector( vec ), mTriangles( triangles ), mVertices( vertices ){
	}
	~Query(){
		mTriangles = 0;
		mVertices = 0;
	}
	const Vector3& mBegin;
	const Vector3& mVector;
	const Triangle* mTriangles;
	const Vector3* mVertices;
private:
	void operator=( const Query& ); //ない
};

//ノード構築をする際のbuild()に渡す引数を減らすための構造体
class Node;
struct BuildArgs{
	BuildArgs( 
	const Triangle* triangles,
	const Vector3* vertices,
	Node* nodes,
	int triangleNumber ) :
	mTriangles( triangles ),
	mVertices( vertices ),
	mNodePos( nodes ),
	mIndexPool( triangleNumber ),
	mHitFlags( 0 ){
		mHitFlags = NEW bool[ triangleNumber * 2 ];
	}
	~BuildArgs(){
		SAFE_DELETE_ARRAY( mHitFlags );
		mTriangles = 0;
		mVertices = 0;
		mNodePos = 0;
	}

	const Triangle* mTriangles;
	const Vector3* mVertices;
	Node* mNodePos; //ノード配列ポインタ
	Pool< int > mIndexPool; //インデクス確保用プール
	bool* mHitFlags; //一時保管用ヒットフラグ領域
private:
	void operator=( const BuildArgs& ); //ない
};

bool getIntersectionTriangleAndLineSegment( 
float* tOut, 
const Query& q,
int index ){ //三角形インデクス
	const Triangle& tri = q.mTriangles[ index ];
	int i0 = tri.mIndices[ 0 ];
	int i1 = tri.mIndices[ 1 ];
	int i2 = tri.mIndices[ 2 ];
	const Vector3& p0 = q.mVertices[ i0 ];
	const Vector3& p1 = q.mVertices[ i1 ];
	const Vector3& p2 = q.mVertices[ i2 ];

	Vector3 minusD, minusE; //辺の逆向き
	Vector3 f;
	f.setSub( p0, q.mBegin ); 
	minusD.setSub( p0, p1 );
	minusE.setSub( p0, p2 );
	//ここでは連立方程式を解いて一発で求める。本文18.3.2参照のこと。
	//| bx -dx -ex | | ox | = | fx |
	//| by -dy -ey | | oy | = | fy |
	//| bz -dz -ez | | oz | = | fz |
	//bt+du+ev=p0-a を解けば良い。
	Vector3 tuv; //解
	if ( solveLinearSystem( &tuv, q.mVector, minusD, minusE, f ) ){
		*tOut = tuv.x; //tを返す
		return true;
	}else{
		return false; //外れ
	}
}

class Node{
public:
	struct InnerNode{ //内部ノードのメンバ
		float mLine; //分割線
		Node* mLeft; //小さい側
		Node* mRight; //大きい側
	};
	struct OuterNode{ //末端ノードのメンバ。三角形を持つのは末端ノードだけである。
		int* mIndices; //三角形番号配列
		int mIndexNumber;
	};
	Node() : mAxis( -1 ){
		mInner.mLine = 0.f;
		mInner.mLeft = mInner.mRight = 0;
		mOuter.mIndices = 0;
		mOuter.mIndexNumber = 0;
	}
	//再帰判定関数
	bool isIntersect( 
	const Query& q,
	float beginT, //a,bはオリジナルのままで、箱のサイズに合わせるのはa'=a+beginT*b,b'=a+endT*b,と計算して行う。
	float endT ) const {
		//外点であれば単に総当り
		if ( mAxis == -1 ){ //分割軸が-1は外点のしるし
			for ( int i = 0; i < mOuter.mIndexNumber; ++i ){
				int triIdx = mOuter.mIndices[ i ];
++G;
				float dummyT;
				if ( getIntersectionTriangleAndLineSegment( &dummyT, q, triIdx ) ){ //オリジナル線分で判定する。
					return true;
				}
			}
		}else{ //内点。
			//別名
			const Vector3& a = q.mBegin;
			const Vector3& b = q.mVector;
			const Node* l = mInner.mLeft;
			const Node* r = mInner.mRight;
			//平面に当たる時刻を求める。 d = a + bt より t = ( d - a ) / b
			float divT;
			if ( b[ mAxis ] == 0.f ){ //分母0
				divT = FLOAT_MAX;
			}else{
				divT = ( mInner.mLine - a[ mAxis ] ) / b[ mAxis ];
			}
			float endT0, beginT1; //最初に行くノードの終端時刻、後に行くノードの始点時刻
			if ( divT >= endT ){ //後ろのノードまで到達しない
				endT0 = endT;
				beginT1 = endT;
			}else if ( divT > beginT ){ //途中で次のノードに突入する
				//演算誤差がなければdivTをそのまま渡せばいいが、誤差が怖いのでとりあえず十分広めに取る。
				endT0 = ( endT + divT * 15.f ) * ( 1.f / 16.f );
				beginT1 = ( beginT + divT * 15.f ) * ( 1.f / 16.f ); // 1/16だけ始点と終点を混ぜる。0.001などを足すと範囲が狭くなった時に両端を越えてしまうからだ。
			}else{ //後ろのノードへは向かわない
				endT0 = endT;
				beginT1 = endT;
			}
			//始点を求める。1要素で良い
			float begin = a[ mAxis ] + beginT * b[ mAxis ];
			//誤差を計算。使っている項いずれも相対誤差は機械精度のままである。beginTは過剰に余裕を取っている。
			float e = GameLib::Math::abs( a[ mAxis ] ) + GameLib::Math::abs( beginT * b[ mAxis ] ) + GameLib::Math::abs( mInner.mLine );
			e *= EPSILON * 10.f; //が、念のため10倍
			//始点:左
			if ( begin + e < mInner.mLine ){
				if ( l && l->isIntersect( q, beginT, endT0 ) ){
					return true;
				}
				if ( beginT1 < endT ){
					if ( r && r->isIntersect( q, beginT1, endT ) ){
						return true;
					}
				}
			}else if ( begin - e > mInner.mLine ){ //始点:右
				if ( r && r->isIntersect( q, beginT, endT0 ) ){
					return true;
				}
				if ( beginT1 < endT ){
					if ( l && l->isIntersect( q, beginT1, endT ) ){
						return true;
					}
				}
			}else{ //どっちかわからない。
				//レアケースなので時間範囲は丸ごととする。
				if ( l && l->isIntersect( q, beginT, endT ) ){
					return true;
				}
				if ( r && r->isIntersect( q, beginT, endT ) ){
					return true;
				}
			}
		}
		return false;
	}
	//再帰判定関数。戻り値は三角形番号
	void getIntersection(
	int* minIndex, //最小時刻で当たった三角形番号
	float* minT, //最小時刻
	const Query& q,
	float beginT, //a,bはオリジナルのままで、箱のサイズに合わせるのはa'=a+beginT*b,b'=a+endT*b,と計算して行う。
	float endT ) const {
		//外点であれば単に総当り
		if ( mAxis == -1 ){ //分割軸が-1は外点のしるし
			for ( int i = 0; i < mOuter.mIndexNumber; ++i ){
				int triIdx = mOuter.mIndices[ i ];
++G;
				float t;
				if ( getIntersectionTriangleAndLineSegment( &t, q, triIdx ) ){ //オリジナル線分で判定する。
					if ( t < *minT ){
						*minT = t; //更新
						*minIndex = triIdx;
					}
				}
			}
		}else{ //内点。
			//別名
			const Vector3& a = q.mBegin;
			const Vector3& b = q.mVector;
			const Node* l = mInner.mLeft;
			const Node* r = mInner.mRight;
			//平面に当たる時刻を求める。 d = a + bt より t = ( d - a ) / b
			float divT;
			if ( b[ mAxis ] == 0.f ){ //分母0
				divT = FLOAT_MAX;
			}else{
				divT = ( mInner.mLine - a[ mAxis ] ) / b[ mAxis ];
			}
			float endT0, beginT1; //最初に行くノードの終端時刻、後に行くノードの始点時刻
			if ( divT >= endT ){ //後ろのノードまで到達しない
				endT0 = endT;
				beginT1 = endT;
			}else if ( divT > beginT ){ //途中で次のノードに突入する
				//演算誤差がなければdivTをそのまま渡せばいいが、誤差が怖いのでとりあえず十分広めに取る。
				endT0 = ( endT + divT * 15.f ) * ( 1.f / 16.f );
				beginT1 = ( beginT + divT * 15.f ) * ( 1.f / 16.f ); // 1/16だけ始点と終点を混ぜる
			}else{ //後ろのノードへは向かわない
				endT0 = endT;
				beginT1 = endT;
			}
			//始点を求める。1要素で良い
			float begin = a[ mAxis ] + beginT * b[ mAxis ];
			//誤差を計算。使っている項いずれも相対誤差は機械精度のままである。
			float e = GameLib::Math::abs( a[ mAxis ] ) + GameLib::Math::abs( beginT * b[ mAxis ] ) + GameLib::Math::abs( mInner.mLine );
			e *= EPSILON * 10.f; //が、念のため10倍
			//始点:左
			if ( begin + e < mInner.mLine ){
				if ( l ){
					l->getIntersection( minIndex, minT, q, beginT, endT0 );
				}
				if ( beginT1 < endT ){
					if ( r ){
						r->getIntersection( minIndex, minT, q, beginT1, endT );
					}
				}
			}else if ( begin - e > mInner.mLine ){ //始点:右
				if ( r ){
					r->getIntersection( minIndex, minT, q, beginT, endT0 );
				}
				if ( beginT1 < endT ){
					if ( l ){
						l->getIntersection( minIndex, minT, q, beginT1, endT );
					}
				}
			}else{ //どっちかわからない。
				//レアケースなので時間範囲は丸ごととする。
				if ( l ){
					l->getIntersection( minIndex, minT, q, beginT, endT );
				}
				if ( r ){
					r->getIntersection( minIndex, minT, q, beginT, endT );
				}
			}
		}
	}
	//再帰構築関数
	/*
	分割の際、分割線の計算には誤差が伴うが、誤差込みで出来た分割線を採用する。
	つまり、0.5になるべき分割線が0.51になるのであれば、0.51を分割線とする。
	分割線として何を使うかはアルゴリズム上任意だからである。
	そして三角形の頂点座標は元データとして扱い、誤差の混入は仮定しない。
	かつ、三角形の座標に関しては比較以外の演算を行わず誤差は混入しない。
	よって、各三角形が分割線に対して左右どちらにあるかは厳密に誤差を考えずに決めてよい。
	なお、分割線に厳密に乗る三角形は「右にある」とする。
	*/
	void build(
	const Vector3& minV, //範囲最小値
	const Vector3& maxV, //範囲最大値
	BuildArgs* args,
	int restLevel ){ //あと何回分割するか
		//最初はOuterだけを使う
		OuterNode* outer = &mOuter;

		//最良の分割をどう決めるかは難しい問題だが、ここでは最も長い方向を割る。これが平均的には妥当な結果を出す。
		int axis = -1; //分割軸
		Vector3 size;
		size.setSub( maxV, minV );
		float maxSize = -FLOAT_MAX;
		for ( int i = 0; i < 3; ++i ){
			if ( size[ i ] > maxSize ){
				maxSize = size[ i ];
				axis = i;
			}
		}
		//分割線
		float div;
		div = minV[ axis ] + maxV[ axis ];
		div *= 0.5f;
		//判定結果は後で使うのでbool配列に保持しておく。領域はnewすると遅いので、引数でもらっている。
		bool* hitL = args->mHitFlags;
		bool* hitR = hitL + outer->mIndexNumber;
		//左右の三角形数
		int cl = 0;
		int cr = 0;
		//どちらに属すか判定
		const Triangle* triangles = args->mTriangles;
		const Vector3* vertices = args->mVertices;
		for ( int i = 0; i < outer->mIndexNumber; ++i ){ //各三角形について調べる
			const Triangle& tri = triangles[ outer->mIndices[ i ] ];
			const Vector3& p0 = vertices[ tri.mIndices[ 0 ] ];
			const Vector3& p1 = vertices[ tri.mIndices[ 1 ] ];
			const Vector3& p2 = vertices[ tri.mIndices[ 2 ] ];
			//軸について最小最大を取得
			float minP, maxP;
			minP = min( p0[ axis ], p1[ axis ] );
			minP = min( minP, p2[ axis ] );
			maxP = max( p0[ axis ], p1[ axis ] );
			maxP = max( maxP, p2[ axis ] );

			hitL[ i ] = ( minP < div ); //明らかに左に所属 誤差を考慮していない理由は関数の前にあるコメントを参照のこと。
			hitR[ i ] = ( maxP > div ); //明らかに右に所属
			//分割線ぴったりである場合。この場合右に所属させる。両方に所属させるとかぶるからだ。
			//誤差の考慮は判定時に行う。
			if ( ( minP == maxP ) && ( minP == div ) ){
				hitR[ i ] = true;
			}
			if ( hitL[ i ] ){
				++cl;
			}
			if ( hitR[ i ] ){
				++cr;
			}
		}
		//分割開始
		//ここでInnerNodeに変わる。OuterNodeのメンバは一旦ローカル変数に退避
		//unionは頭を混乱させるのであまりつかうべきではないが、性能上の理由があれば仕方あるまい。
		int* indices = outer->mIndices;
		int indexNumber = outer->mIndexNumber;
		outer->mIndices = 0;
		outer->mIndexNumber = 0;
		outer = 0; //これ以降Outerはつかえない

		InnerNode* inner = &mInner; //ここ以降innerのみにアクセス

		//mAxisに-1以外が入っていることがInnerNodeであるしるしだ。
		mAxis = axis; //これでaxis==0ならそのまま、1ならY、2ならZになる
		inner->mLine = div; //分割線を書き込み

		//左右ノードの確保
		if ( cl > 0 ){ //三角形があれば
			inner->mLeft = args->mNodePos;
#ifdef STRONG_DEBUG
Vector3 boxDiv = maxV;
boxDiv[ axis ] = div;
inner->mLeft->mMin = minV;
inner->mLeft->mMax = boxDiv;
inner->mLeft->mOrigN = cl;
#endif
			++( args->mNodePos );
			inner->mLeft->mOuter.mIndices = args->mIndexPool.allocate( cl );
		}
		if ( cr > 0 ){
			inner->mRight = args->mNodePos;
#ifdef STRONG_DEBUG
Vector3 boxDiv = minV;
boxDiv[ axis ] = div;
inner->mRight->mMin = boxDiv;
inner->mRight->mMax = maxV;
inner->mRight->mOrigN = cr;
#endif
			++( args->mNodePos );
			inner->mRight->mOuter.mIndices = args->mIndexPool.allocate( cr );
		}
		//左右ノードに振り分け。
		for ( int i = 0; i < indexNumber; ++i ){ //各三角形について調べる
			if ( hitL[ i ] ){
				OuterNode* l = &inner->mLeft->mOuter;
				l->mIndices[ l->mIndexNumber ] = indices[ i ];
				++( l->mIndexNumber );
			}
			if ( hitR[ i ] ){
				OuterNode* r = &inner->mRight->mOuter;
				r->mIndices[ r->mIndexNumber ] = indices[ i ];
				++( r->mIndexNumber );
			}
		}
		//ヒットフラグ領域を使いまわす関係で、再帰呼び出しは振り分けが終わってから。
		if ( restLevel > 1 ){ //最終レベルはもう呼ばない
			if ( cl > 1 ){
				Vector3 boxDiv = maxV;
				boxDiv[ axis ] = div;
				inner->mLeft->build( minV, boxDiv, args, restLevel - 1 );
			}
			if ( cr > 1 ){
				Vector3 boxDiv = minV;
				boxDiv[ axis ] = div;
				inner->mRight->build( boxDiv, maxV, args, restLevel - 1 );
			}
		}
	}
	void count( int* nodeNumber, int* indexNumber ){
		++( *nodeNumber ); //自分の分を加算
		if ( mAxis != -1 ){
			if ( mInner.mLeft ){
				mInner.mLeft->count( nodeNumber, indexNumber );
			}
			if ( mInner.mRight ){
				mInner.mRight->count( nodeNumber, indexNumber );
			}
		}else{
			*indexNumber += mOuter.mIndexNumber;
		}		
	}
	Node* copy( Node** nodePos, int** indexPos ){
		Node* to = *nodePos; //コピー先
		++( *nodePos );
#ifdef STRONG_DEBUG
to->mMin = mMin;
to->mMax = mMax;
to->mOrigN = mOrigN;
#endif
		to->mAxis = mAxis;
		if ( mAxis != -1 ){ //内部ノードの場合
			to->mInner.mLine = mInner.mLine;
			if ( mInner.mLeft ){
				to->mInner.mLeft = mInner.mLeft->copy( nodePos, indexPos );
			}
			if ( mInner.mRight ){
				to->mInner.mRight = mInner.mRight->copy( nodePos, indexPos );
			}
		}else{ //インデクス確保
			to->mOuter.mIndices = *indexPos;
			//インデクスをコピー
			for ( int i = 0; i < mOuter.mIndexNumber; ++i ){
				to->mOuter.mIndices[ i ] = mOuter.mIndices[ i ];
			}
			to->mOuter.mIndexNumber = mOuter.mIndexNumber;
			( *indexPos ) += mOuter.mIndexNumber;
		}
		return to;
	}
	//下の方の無駄なノードを削除する。削るのは以下の3種類
	//1.左が外点で右が空。左の内容を自分に引き上げる。
	//2.右が外点で左が空。右の内容を自分に引き上げる。
	//3.左右が外点で三角形リストが同一。子を消して自分にコピーする。
	void optimize(){
		if ( mAxis == -1 ){ //外点なら処理なし。終了。
			return;
		}
		Node* l = mInner.mLeft;
		Node* r = mInner.mRight;
		//まず再帰呼び出し
		if ( l ){
			l->optimize();
		}
		if ( r ){
			r->optimize();
		}
		bool lIsOuter = ( l && ( l->mAxis == -1 ) );
		bool rIsOuter = ( r && ( r->mAxis == -1 ) );
		OuterNode* o = 0; //引き上げ外点があればここに入れる。
		if ( lIsOuter && !r ){ //左が外点で右が空。
			o = &l->mOuter;
		}else if ( rIsOuter && !l ){ //右が外点で左が空
			o = &r->mOuter;
		}else if ( lIsOuter && rIsOuter ){ //同一チェック
			OuterNode* lo = &l->mOuter;
			OuterNode* ro = &r->mOuter;
			bool match = true;
			if ( lo->mIndexNumber == ro->mIndexNumber ){
				for ( int i = 0; i < lo->mIndexNumber; ++i ){
					if ( lo->mIndices[ i ] != ro->mIndices[ i ] ){
						match = false;
					}
				}
				if ( match ){ //全部マッチ。左を自分に引き上げる。
					o = lo;
				}
			}
		}
		if ( o ){ //引き上げ情報あり。自分を外点にする。
			mAxis = -1;
			mInner.mLeft = mInner.mRight = 0;
			mOuter.mIndexNumber = o->mIndexNumber;
			mOuter.mIndices = o->mIndices;
		}
	}
	void checkTriangleIndexExistance( Array< bool >& f ){
		if ( mAxis == -1 ){
			for ( int i = 0; i < mOuter.mIndexNumber; ++i ){
				f[ mOuter.mIndices[ i ] ] = true;
			}
		}else{
			if ( mInner.mLeft ){
				mInner.mLeft->checkTriangleIndexExistance( f );
			}
			if ( mInner.mRight ){
				mInner.mRight->checkTriangleIndexExistance( f );
			}
		}
	}
	void checkTriangle( const Triangle* triangles, const Vector3* vertices ) const {
		if ( mAxis == -1 ){ //内点のみチェック
			return;
		}
		if ( mInner.mLeft ){
			if ( mInner.mLeft->mAxis == -1 ){
				const OuterNode& o = mInner.mLeft->mOuter;
				for ( int i = 0; i < o.mIndexNumber; ++i ){
					int triIdx = o.mIndices[ i ];
					const Triangle& tri = triangles[ triIdx ];
					bool hit = false;
					for ( int j = 0; j < 3; ++j ){
						const Vector3& p = vertices[ tri.mIndices[ j ] ];
						for ( int k = 0; k < 3; ++k ){
							if ( p[ k ] < mInner.mLine ){
								hit = true;
							}
						}
					}
					STRONG_ASSERT( hit );
				}
			}else{
				mInner.mLeft->checkTriangle( triangles, vertices );
			}
		}
		if ( mInner.mRight ){
			if ( mInner.mRight->mAxis == -1 ){
				const OuterNode& o = mInner.mRight->mOuter;
				for ( int i = 0; i < o.mIndexNumber; ++i ){
					int triIdx = o.mIndices[ i ];
					const Triangle& tri = triangles[ triIdx ];
					bool hit = false;
					for ( int j = 0; j < 3; ++j ){
						const Vector3& p = vertices[ tri.mIndices[ j ] ];
						for ( int k = 0; k < 3; ++k ){
							if ( p[ k ] >= mInner.mLine ){
								hit = true;
							}
						}
					}
					STRONG_ASSERT( hit );
				}
			}else{
				mInner.mRight->checkTriangle( triangles, vertices );
			}
		}
	}
#ifdef STRONG_DEBUG
Vector3 mMin; //デバグ用
Vector3 mMax;
int mOrigN;
#endif
	int mAxis; //分割軸。0:X, 1:Y, 2:Z, -1はOuterNode。
	union{ //unionの中はメモリが共有される。末端と内部では必要なメンバが違うので性能向上のためにこうしている。
		InnerNode mInner;
		OuterNode mOuter;
	};
};

} //namespace{}

namespace GameLib{
namespace Scene{

class CollisionMesh::Impl : public ReferenceType {
public:
	explicit Impl( const char* filename ) :
	mVertices( 0 ), 
	mVertexNumber( 0 ),
	mTriangles( 0 ),
	mTriangleNumber( 0 ),
	mNodes( 0 ),
	mIndicesInNode( 0 ){
		mDocument = Document::create( filename );
	}
	explicit Impl( ConstElement e ) : 
	mVertices( 0 ), 
	mVertexNumber( 0 ),
	mTriangles( 0 ),
	mTriangleNumber( 0 ),
	mNodes( 0 ),
	mIndicesInNode( 0 ){
		createFromElement( e );
	}
	~Impl(){
		SAFE_DELETE_ARRAY( mVertices );
		SAFE_DELETE_ARRAY( mTriangles );
		SAFE_DELETE_ARRAY( mNodes );
		SAFE_DELETE_ARRAY( mIndicesInNode );
	};
	void createFromElement( ConstElement e ){
		//第一パス。頂点数、三角形数を数える。
		int n = e.childNumber();
		for ( int i = 0; i < n; ++i ){
			ConstElement child = e.child( i );
			RefString name( child.name() );
			if ( name == "VertexBuffer" ){
				mVertexNumber += child.childNumber();
			}else if ( name == "IndexBuffer" ){
				mTriangleNumber += child.childNumber();
			}
		}
		//確保
		mVertices = NEW Vector3[ mVertexNumber ];
		mTriangles = NEW Triangle[ mTriangleNumber ];
		//第二パス。頂点充填
		int vertexPos = 0;
		int trianglePos = 0;
		for ( int i = 0; i < n; ++i ){
			ConstElement child = e.child( i );
			RefString name( child.name() );
			if ( name == "VertexBuffer" ){
				vertexPos = fillVertices( child, vertexPos );
			}else if ( name == "IndexBuffer" ){
				trianglePos = fillTriangles( child, trianglePos );
			}
		}
		//第三パスk-d tree生成
		buildKDTree();
	}
	void buildKDTree(){
		//ノードは三角形が理想的に1個づつ格納されたと仮定したところまで割る数だけ用意する。
		//三角形数tn以上最小の2の冪を求め、その二倍だけ用意すればよい。
		int tn = mTriangleNumber;
		int maxNodeNumber = 1;
		int maxLevel = 0; //最大分割数
		while ( maxNodeNumber < tn ){
			maxNodeNumber <<= 1;
			++maxLevel;
		}
		Node* tmpNodes = NEW Node[ maxNodeNumber * 2 ];
		//引数構造体準備
		BuildArgs args( mTriangles, mVertices, tmpNodes, mTriangleNumber );

		Node root;
		//初期三角形番号充填
		root.mOuter.mIndices = args.mIndexPool.allocate( mTriangleNumber );
		root.mOuter.mIndexNumber = mTriangleNumber;
		for ( int i = 0; i < mTriangleNumber; ++i ){
			root.mOuter.mIndices[ i ] = i;
		}
		//世界のサイズを測定。XYZの最大最小を調べる。
		Vector3 minV( FLOAT_MAX ); //最大で初期化
		Vector3 maxV( -minV.x ); //最小で初期化
		for ( int i = 0; i < mVertexNumber; ++i ){
			minV.min( mVertices[ i ] ); //minはXYZで最小値を取って自分に入れる。
			maxV.max( mVertices[ i ] ); //maxはXYZで最大値を取って自分に入れる。
		}
		root.build( minV, maxV, &args, maxLevel );
#ifdef STRONG_DEBUG
root.mMin = minV;
root.mMax = maxV;
#endif
		root.optimize(); //無用なノードを削って性能を上げる。
		//次に必要な領域を測定して、固定した領域にコピーする。
		int indexNumber = 0;
		int nodeNumber = 0;
		root.count( &nodeNumber, &indexNumber );
		mIndicesInNode = NEW int[ indexNumber ];
		mNodes = NEW Node[ nodeNumber ];
		//全ノードをコピー
		Node* nodePos = mNodes;
		int* indicesPos = mIndicesInNode;
		root.copy( &nodePos, &indicesPos );
		//後始末
		SAFE_DELETE_ARRAY( tmpNodes );
#ifdef STRONG_DEBUG
		Array< bool > triangleExistFlags( mTriangleNumber );
		for ( int i = 0; i < mTriangleNumber; ++i ){
			triangleExistFlags[ i ] = false;
		}
		mNodes[ 0 ].checkTriangleIndexExistance( triangleExistFlags );
		for ( int i = 0; i < mTriangleNumber; ++i ){
			STRONG_ASSERT( triangleExistFlags[ i ] );
		}
		mNodes[ 0 ].checkTriangle( mTriangles, mVertices );
#endif
	}
	bool isReady(){
		bool ok = true;
		if ( mDocument ){
			if ( mDocument.isReady() ){
				ConstElement e = mDocument.root();
				//子供が一人で名前がContainerの時に限って、子を渡す。
				if ( ( e.childNumber() == 1 ) && ( RefString( "Container" ) != e.name() ) ){
					e = e.child( 0 );
				}
				createFromElement( e );
				e.release(); //先に破棄。
				mDocument.release();
			}else{
				ok = false;
			}
		}
		return ok;
	}
	bool isIntersect( const Vector3& a, const Vector3& b ) const {
		Query q( a, b, mTriangles, mVertices );
		bool ret = mNodes[ 0 ].isIntersect( q, 0.f, 1.f );
#ifdef STRONG_DEBUG //比較用総当り版
		bool retCheck = false;
		for ( int i = 0; i < mTriangleNumber; ++i ){
			float dummyT;
			if ( getIntersectionTriangleAndLineSegment( &dummyT, q, i ) ){
				retCheck = true;
			}
		}
		STRONG_ASSERT( ret == retCheck );
#endif
		return ret;
	}
	bool getIntersection(
	float* tOut,
	Vector3* nOut,
	const Vector3& a,
	const Vector3& b ) const {
G = 0;
		float t = FLOAT_MAX; //ここから出発
		int index = -1;
		Query q( a, b, mTriangles, mVertices );
		mNodes[ 0 ].getIntersection( &index, &t, q, 0.f, 1.f );
#ifdef STRONG_DEBUG //比較用総当り版
		int indexCheck = -1;
		float tCheck = FLOAT_MAX;
		for ( int i = 0; i < mTriangleNumber; ++i ){
			float tmpT;
			if ( getIntersectionTriangleAndLineSegment( &tmpT, q, i ) ){
				if ( tmpT < tCheck ){
					tCheck = tmpT;
					indexCheck = i;

				}
			}
		}
		STRONG_ASSERT( t == tCheck ); //indexは合わないことがある。
#endif
		if ( index != -1 ){
			//最終的に当たった三角形の情報を格納
			if ( tOut ){
				*tOut = t;
			}
			if ( nOut ){
				int i0 = mTriangles[ index ].mIndices[ 0 ];
				int i1 = mTriangles[ index ].mIndices[ 1 ];
				int i2 = mTriangles[ index ].mIndices[ 2 ];
				const Vector3& p0 = mVertices[ i0 ];
				const Vector3& p1 = mVertices[ i1 ];
				const Vector3& p2 = mVertices[ i2 ];
				Vector3 e0, e1; //Edge(=辺)
				e0.setSub( p1, p0 );
				e1.setSub( p2, p0 );
				nOut->setCross( e0, e1 );
				nOut->normalize();
			}
			return true;
		}else{
			return false;
		}
	}
private:
	int fillVertices( ConstElement vb, int pos ){
		int n = vb.childNumber();
		RefString positionStr( "position" );
		for ( int i = 0; i < n; ++i ){
			ConstElement v = vb.child( i );
			int an = v.attributeNumber();
			for ( int j = 0; j < an; ++j ){
				ConstAttribute a = v.attribute( j );
				if ( positionStr == a.name() ){
					a.getFloatValues( &mVertices[ pos ].x, 3 );
					++pos;
				}
			}
		}
		return pos;
	}
	int fillTriangles( ConstElement ib, int pos ){
		int n = ib.childNumber();
		for ( int i = 0; i < n; ++i ){
			ConstElement t = ib.child( i );
			ASSERT( t.attributeNumber() > 0 && "IndexBuffer : It's not valid element." );
			ConstAttribute a = t.attribute( 0 );
			a.getIntValues( mTriangles[ pos ].mIndices, 3 );
			++pos;
		}
		return pos;
	}
	Document mDocument;
	Vector3* mVertices;
	int mVertexNumber;
	Triangle* mTriangles;
	int mTriangleNumber;
	Node* mNodes; //0番が根元
	int* mIndicesInNode; //ノード内インデクス親配列
};

CollisionMesh::CollisionMesh( Impl* impl ) : mImpl( impl ){
	if ( mImpl ){
		mImpl->refer();
	}
}

CollisionMesh CollisionMesh::create( const char* filename ){
	CollisionMesh r;
	r.mImpl = NEW Impl( filename );
	return r;
}

CollisionMesh CollisionMesh::create( ConstElement e ){
	CollisionMesh r;
	r.mImpl = NEW Impl( e );
	return r;
}

bool CollisionMesh::isIntersect( const Vector3& a, const Vector3& b ) const {
	return mImpl->isIntersect( a, b );
}

bool CollisionMesh::getIntersection(
float* t,
Vector3* n,
const Vector3& a,
const Vector3& b ) const {
	return mImpl->getIntersection( t, n, a, b );
}

bool CollisionMesh::isReady(){
	return mImpl->isReady();
}

#define TYPE CollisionMesh
#include "GameLib/Base/Impl/ReferenceTypeTemplate.h"

} //namespace Scene
}//namespace GameLib