#include "GameLib/GameLib.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Math/Random.h"
#include <cmath>

namespace GameLib{
namespace Math{

const float TO_RAD = 3.1415926535897932384626433832795f / 180.f;
const float TO_DEG = 180.f / 3.1415926535897932384626433832795f;

float sin( float a ){
	return sinf( a * TO_RAD );
}

float cos( float a ){
	return cosf( a * TO_RAD );
}

float tan( float a ){
	return tanf( a * TO_RAD );
}

float asin( float a ){
	ASSERT( a >= -1.f );
	ASSERT( a <= 1.f );
	return asinf( a ) * TO_DEG;
}

float acos( float a ){
	ASSERT( a >= -1.f );
	ASSERT( a <= 1.f );
	return acosf( a ) * TO_DEG;
}

float atan( float a ){
	return atanf( a ) * TO_DEG;
}

float atan2( float y, float x ){
	return atan2f( y, x ) * TO_DEG;
}

float pow( float a, float b ){
	return powf( a, b );
}

float log2( float a ){
	//底の変換公式を使う。
	//log_a(x) = ln(x) / ln(a)
	//今a=2なので、ln(x) / ln(2)を計算すればいい
	float logE = logf( a );
	static const float rcpLogE2 = 1.4426950408889634073599246810019f;
	return logE * rcpLogE2;
}

float log10( float a ){
	return log10f( a );
}

float abs( float a ){
	return ( a < 0.f ) ? -a : a;
}

float min( float a, float b ){
	return ( a < b ) ? a : b;
}

float max( float a, float b ){
	return ( a > b ) ? a : b;
}

float sqrt( float a ){
	ASSERT( a >= 0.f );
	return sqrtf( a );
}

bool isSpecial( float a ){
	unsigned* p = reinterpret_cast< unsigned* >( &a );
	return ( ( *p & 0x7e000000 ) == 0x7e000000 );
}

} //namespace Math
} //namespace GameLib
