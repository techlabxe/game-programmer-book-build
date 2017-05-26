#include "GameLib/GameLib.h"
#include "GameLib/Scene/CollisionDetector.h"
#include "GameLib/Base/Impl/ReferenceType.h"
#include "GameLib/Base/Tank.h"
#include "GameLib/Base/Pool.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Functions.h"

namespace {
using namespace GameLib;
using namespace GameLib::Scene;
typedef CollisionDetector::Pair Pair; //流用

class Sphere{
public:
	Vector3 mCenter;
	float mRadius;
};

//Node::Buildの引数セット
class Args{
public:
	Args( 
	const Sphere* spheres,
	int sphereNumber ) : 
	mSpheres( spheres ),
	mIndexPool( sphereNumber ),
	mResults( sphereNumber ){
		mHitFlags = NEW bool[ sphereNumber * 2 ];
	}
	~Args(){
		mSpheres = 0;
		SAFE_DELETE_ARRAY( mHitFlags );
	}

	const Sphere* mSpheres;
	Pool< int > mIndexPool;
	Tank< Pair > mResults;
	bool* mHitFlags;
};

//k-d treeの本体
class Node{
public:
	Node() : mIndices( 0 ), mIndexNumber( 0 ){}
	//構築しては端から判定していく
	void build(
	const Vector3& minV, //範囲最小値(左下奥)
	const Vector3& maxV, //範囲最大値(右上前)
	Args* args,
	int restLevel ){
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
		bool* hitR = hitL + mIndexNumber;
		//左右の物体数
		int cl = 0;
		int cr = 0;
		//どちらに属すか判定
		const Sphere* spheres = args->mSpheres;
		for ( int i = 0; i < mIndexNumber; ++i ){ 
			const Sphere& s = spheres[ mIndices[ i ] ];
			//軸について最小最大を取得
			float minP, maxP;
			minP = s.mCenter[ axis ] - s.mRadius;
			maxP = s.mCenter[ axis ] + s.mRadius;
	
			hitL[ i ] = ( minP < div ); //明らかに左に所属 誤差は考慮しない。動くものの場合誤差でどうこうなっても問題ないし、桁落ちが生じる計算もない。
			hitR[ i ] = ( maxP > div ); //明らかに右に所属
			//分割線ぴったりである場合。この場合右に所属させる。両方に所属させるとかぶるからだ。
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

		//ノードはローカル変数である。外点到達と同時に判定を行って戻ってきてしまうから、取っておく必要はない。
		Node left;
		Node right;
		left.mIndices = args->mIndexPool.allocate( cl );
		right.mIndices = args->mIndexPool.allocate( cr );

		for ( int i = 0; i < mIndexNumber; ++i ){ //各三角形について調べる
			if ( hitL[ i ] ){
				left.mIndices[ left.mIndexNumber ] = mIndices[ i ];
				++( left.mIndexNumber );
			}
			if ( hitR[ i ] ){
				right.mIndices[ right.mIndexNumber ] = mIndices[ i ];
				++( right.mIndexNumber );
			}
		}
		ASSERT( ( left.mIndexNumber == cl ) && ( right.mIndexNumber == cr ) );

		//ヒットフラグ領域を使いまわす関係で、再帰呼び出しは振り分けが終わってから。
		if ( restLevel > 1 ){ //最終レベルはもう呼ばない
			if ( cl > 1 ){
				Vector3 boxDiv = maxV;
				boxDiv[ axis ] = div;
				left.build( minV, boxDiv, args, restLevel - 1 );
			}
			if ( cr > 1 ){
				Vector3 boxDiv = minV;
				boxDiv[ axis ] = div;
				right.build( boxDiv, maxV, args, restLevel - 1 );
			}
		}else{
			//さあ判定するよ。
			left.test( args );
			right.test( args );
		}
	}
	void test( Args* args ) const {
		const Sphere* spheres = args->mSpheres;

		for ( int i = 0; i < mIndexNumber; ++i ){
			int i0 = mIndices[ i ];
			const Sphere& s0 = spheres[ mIndices[ i ] ];
			const Vector3& p0 = s0.mCenter;
			float r0 = s0.mRadius;
			for ( int j = i + 1; j < mIndexNumber; ++j ){
				int i1 = mIndices[ j ];
				const Sphere& s1 = spheres[ mIndices[ j ] ];
				const Vector3& p1 = s1.mCenter;
				float r1 = s1.mRadius;
				//球判定。誤差は考えずとも良い
				Vector3 d;
				d.setSub( p1, p0 );
				float rsum = r0 + r1;
				float rsum2 = rsum * rsum;
				float dist2 = d.squareLength();
				if ( dist2 <= rsum2 ){
					Pair* pair = args->mResults.add();
					if ( i0 < i1 ){ //小さい方をmId0とする。
						pair->mId0 = i0;
						pair->mId1 = i1;
					}else{
						pair->mId0 = i1;
						pair->mId1 = i0;
					}
				}
			}
		}
	}
	int* mIndices;
	int mIndexNumber;
};

} //namespace {}

namespace GameLib{
namespace Scene{

class CollisionDetector::Impl : public ReferenceType{
public:
	Impl( int capacity, int maxLevel ) : 
	mSpheres( 0 ),
	mSphereNumber( 0 ),
	mCapacity( capacity ),
	mMaxLevel( maxLevel ){
		mSpheres = NEW Sphere[ mCapacity ];
	}
	~Impl(){
		SAFE_DELETE_ARRAY( mSpheres );
	}
	int add( const Vector3& c, float r ){
		int i = mSphereNumber;
		ASSERT( i < mCapacity );
		mSpheres[ i ].mCenter = c;
		mSpheres[ i ].mRadius = r;
		++mSphereNumber;
		return i;
	}
	void detect( Array< Pair >* resultsOut ){
#if 1
		Node root;
		Args args( mSpheres, mSphereNumber );
		root.mIndexNumber = mSphereNumber;
		root.mIndices = args.mIndexPool.allocate( mSphereNumber );
		//根ノードにデータ充填。同時に範囲を計算
		Vector3 minV( FLOAT_MAX ); //最大で初期化
		Vector3 maxV( -minV.x ); //最小で初期化
		for ( int i = 0; i < mSphereNumber; ++i ){
			const Sphere& s = mSpheres[ i ];
			Vector3 tMin = s.mCenter;
			Vector3 tMax = s.mCenter;
			Vector3 r( s.mRadius );
			tMin -= r;
			tMin += r;
			minV.min( tMin ); //minはXYZで最小値を取って自分に入れる。
			maxV.max( tMax ); //maxはXYZで最大値を取って自分に入れる。
			//番号充填
			root.mIndices[ i ] = i;
		}
		//ノードは球が一個づつ箱に入る数だけ用意するが、後でおそらく変わるだろう。
		//数n以上最小の2の冪を求め、その二倍だけ用意すればよい。
		int n = mSphereNumber;
		int maxNodeNumber = 1;
		int maxLevel = 0; //最大分割数
		while ( maxNodeNumber < n ){
			maxNodeNumber <<= 1;
			++maxLevel;
		}
		root.build( minV, maxV, &args, maxLevel );
		//Tank内の重複を除去する。
		//1.アクセスを高速化するために配列に移す
		int resultSize = args.mResults.size();
		Pair* resultsArray = NEW Pair[ resultSize ];
		int pos = 0;
		while ( !args.mResults.isEnd() ){
			resultsArray[ pos ] = *args.mResults.get();
			++pos;
			args.mResults.toNext();
		}
		//2.一番目がiであるものがいくつあるのかを数える。
		int* counts = NEW int[ mSphereNumber * 2 ];
		int* offsets = counts + mSphereNumber; //一緒に確保。こういうややこしいことをするとバグの元になる
		for ( int i = 0; i < mSphereNumber; ++i ){ //初期化ループ
			counts[ i ] = 0;
		}
		for ( int i = 0; i < resultSize; ++i ){
			++counts[ resultsArray[ i ].mId0 ];
		}
		//2.オフセットに変換
		int offset = 0;
		for ( int i = 0; i < mSphereNumber; ++i ){
			offsets[ i ] = offset;
			offset += counts[ i ];
			counts[ i ] = 0;
		}
		//1.ソート後配列にソートしながら格納
		int* sorted = NEW int[ offset ]; //offsetにちょうど合計が入っている。
		for ( int i = 0; i < resultSize; ++i ){
			int i0 = resultsArray[ i ].mId0;
			sorted[ offsets[ i0 ] + counts[ i0 ] ] = resultsArray[ i ].mId1;
			++counts[ i0 ];
		}
		//箱ごとに重複を除いたものを戻り値にセット
		resultsOut->setSize( offset ); //重複込みで入る数だけ確保
		pos = 0;
		for ( int i = 0; i < mSphereNumber; ++i ){
			int* box = &sorted[ offsets[ i ] ];
			int n = counts[ i ];
			for ( int j = 0; j < n; ++j ){
				if ( box[ j ] >= 0 ){
					( *resultsOut )[ pos ].mId0 = i;
					( *resultsOut )[ pos ].mId1 = box[ j ];
					++pos;
					for ( int k = j + 1; k < n; ++k ){ //同じのが後ろにいればつぶす。
						if ( box[ j ] == box[ k ] ){
							box[ k ] = -1;
						}
					}
				}
			}
		}
		//最終結果のサイズを縮小。引越しは起きない。
		resultsOut->setSize( pos );
		//後始末
		SAFE_DELETE_ARRAY( sorted );
		SAFE_DELETE_ARRAY( counts );
		SAFE_DELETE_ARRAY( resultsArray );
#else //総当り
		Tank< Pair > results( mSphereNumber );
		for ( int i = 0; i < mSphereNumber; ++i ){
			const Sphere& s0 = mSpheres[ i ];
			const Vector3& p0 = s0.mCenter;
			float r0 = s0.mRadius;
			for ( int j = i + 1; j < mSphereNumber; ++j ){
				const Sphere& s1 = mSpheres[ j ];
				const Vector3& p1 = s1.mCenter;
				float r1 = s1.mRadius;
				//球判定。誤差は後で考える
				Vector3 d;
				d.setSub( p1, p0 );
				float rsum = r0 + r1;
				float rsum2 = rsum * rsum;
				float dist2 = d.getSquareLength();
				if ( dist2 <= rsum2 ){
					Pair* pair = results.add();
					pair->mId0 = i;
					pair->mId1 = j;
				}
			}
		}
		//配列に変換
		results.copyTo( resultsOut );
#endif
		//判定後に球の個数を0に。
		mSphereNumber = 0;
	}
private:
	void operator=( const Impl& ); //禁止

	Sphere* mSpheres;
	int mSphereNumber;
	const int mCapacity;
	const int mMaxLevel;
};

CollisionDetector::CollisionDetector( Impl* impl ) : mImpl( impl ){
	if ( mImpl ){
		mImpl->refer();
	}
}

CollisionDetector CollisionDetector::create( int capacity, int maxLevel ){
	CollisionDetector r;
	r.mImpl = NEW Impl( capacity, maxLevel );
	return r;
}

int CollisionDetector::add( const Vector3& sphereCenter, float sphereRadius ){
	return mImpl->add( sphereCenter, sphereRadius );
}

void CollisionDetector::detect( Array< Pair >* resultOut ){
	mImpl->detect( resultOut );
}

#define TYPE CollisionDetector
#include "GameLib/Base/Impl/ReferenceTypeTemplate.h"

} //namespace Scene
} //namespace GameLib
