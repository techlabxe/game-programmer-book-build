#include "GameLib/GameLib.h"
#include "GameLib/Math/Random.h"
#include "GameLib/Base/Impl/ReferenceType.h"

namespace GameLib{
namespace Math{

//XorShift法
class Random::Impl : public ReferenceType{
public:
	Impl( int seed ){
		setSeed( seed );
	}
	void setSeed( int seed ){
		mX = 123456789;
		mY = 362436069;
		mZ = 521288629;
		mW = ( seed <= 0 ) ? 88675123 : seed;
	}
	int getInt(){
		unsigned t = mX ^ ( mX << 11 );
		mX = mY;
		mY = mZ;
		mZ = mW;
		mW = ( mW ^ ( mW >> 19 ) ) ^ ( t ^ ( t >> 8 ) );
		return static_cast< int >( mW );
	}
	int getInt( int a ){
		int x = getInt() & 0x7fffffff; //符号ビット切捨て。精度は落ちるがこうしないと範囲に入らない
		x %= a;
		return x;
	}
	int getInt( int a, int b ){
		int x = getInt() & 0x7fffffff; //符号ビット切捨て。精度は落ちるがこうしないと範囲に入らない
		x %= b - a;
		x += a;
		return x;
	}
	float getFloat(){
		int x = getInt();
		x = ( x & 0x007fffff ) | 0x3f800000; //指数部に固定で127をぶっこむ
		float f = *reinterpret_cast< float* >( &x ); //無理やりキャスト
		return f - 1.f; //上の行で1-2になっている
	}
	float getFloat( float a ){
		float f = getFloat();
		f *= a;
		return f;
	}
	float getFloat( float a, float b ){
		float f = getFloat();
		f *= b - a;
		f += a;
		return f;
	}
private:
	unsigned mX;
	unsigned mY;
	unsigned mZ;
	unsigned mW;
};

Random Random::create( int seed ){
	Random r;
	r.mImpl = NEW Impl( seed );
	return r;
}

void Random::setSeed( int seed ){
	mImpl->setSeed( seed );
}

int Random::getInt(){
	return mImpl->getInt();
}

int Random::getInt( int a ){
	return mImpl->getInt( a );
}

int Random::getInt( int a, int b ){
	return mImpl->getInt( a, b );
}

float Random::getFloat(){
	return mImpl->getFloat();
}

float Random::getFloat( float a ){
	return mImpl->getFloat( a );
}

float Random::getFloat( float a, float b ){
	return mImpl->getFloat( a, b );
}

#define TYPE Random
#include "GameLib/Base/Impl/ReferenceTypeTemplate.h"

} //namespace Math
} //namespace GameLib