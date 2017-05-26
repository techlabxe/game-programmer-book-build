#include "GameLib/Framework.h"
#include "GameLib/DebugScreen.h"
#include "GameLib/Math/Random.h"
#include <algorithm>
#include <cmath>
using namespace std;
using namespace GameLib;
using namespace GameLib::Math;

//ランダム性の検定
//32*32*32の箱にランダムに入れていき、ちゃんとランダムかを確認する。
Random gRandom;

const int N = 1000*1000;
const int M = 32;
const int M3 = M * M * M;
double gK = 0.0;
double gC = 0.0;

namespace GameLib{
	void Framework::configure( Configuration* c ){
		c->enableVSync( false );
	}
	void Framework::update(){
		if ( !gRandom ){
			gRandom = Random::create();
		}
		int* b = new int[ M3 ];
		for ( int i = 0; i < M3; ++i ){
			b[ i ] = 0;
		}
		for ( int i = 0; i < N; ++i ){
			int v[ 3 ];
			for ( int j = 0; j < 3; ++j ){
#if 0 //1にすると標準rand(),0にするとMath::Random
				v[ j ] = rand();
#else
				v[ j ] = gRandom.getInt();
#endif
				v[ j ] &= 0x1f;
			}
			++b[ ( ( ( v[ 0 ] * M ) + v[ 1 ] ) * M ) + v[ 2 ] ];
		}
		double s = 0.0;
		double a = static_cast< double >( N ) / M3;
		for ( int i = 0; i < M3; ++i ){
			s += ( b[ i ] - a ) * ( b[ i ] - a );
		}
		s /= M3;
		s = std::sqrt( s );
		double sIdeal = std::sqrt( a );
		SAFE_DELETE_ARRAY( b );

		gK += s - sIdeal;
		gC += 1.0;

		//順に計算した標準偏差、理想的標準偏差、理想との差、理想との差(累積)
		DebugScreen sc;
		sc << s << " " << sIdeal << " " << s - sIdeal << " " << gK/gC;
	}
}
