
#include "Cuboid.h"
#include "Library/Vector3.h"

namespace{ //このcppでしか見えない変数

const double INTERNAL_UNIT = 1000.0; //内部単位は1000倍。1ミリ単位。

} //namespace{}

void Cuboid::setPosition( const Vector3& a ){
	mX = static_cast< int >( a.x * INTERNAL_UNIT );
	mY = static_cast< int >( a.y * INTERNAL_UNIT );
	mZ = static_cast< int >( a.z * INTERNAL_UNIT );
}

void Cuboid::setHalfSize( const Vector3& a ){
	mHalfSizeX = static_cast< int >( a.x * INTERNAL_UNIT );
	mHalfSizeY = static_cast< int >( a.y * INTERNAL_UNIT );
	mHalfSizeZ = static_cast< int >( a.z * INTERNAL_UNIT );
}

bool Cuboid::isIntersect( const Cuboid& b ) const {
	int ax0 = mX - mHalfSizeX; //left A
	int ax1 = mX + mHalfSizeX; //right A
	int bx0 = b.mX - b.mHalfSizeX; //left B
	int bx1 = b.mX + b.mHalfSizeX; //right B
	if ( ( ax0 < bx1 ) && ( ax1 > bx0 ) ){
		int ay0 = mY - mHalfSizeY; //top A
		int ay1 = mY + mHalfSizeY; //bottom A
		int by0 = b.mY - b.mHalfSizeY; //top B
		int by1 = b.mY + b.mHalfSizeY; //bottom B
		if ( ( ay0 < by1 ) && ( ay1 > by0 ) ){
			int az0 = mZ - mHalfSizeZ; //front A
			int az1 = mZ + mHalfSizeZ; //back A
			int bz0 = b.mZ - b.mHalfSizeZ; //front B
			int bz1 = b.mZ + b.mHalfSizeZ; //back B
			if ( ( az0 < bz1 ) && ( az1 > bz0 ) ){
				return true;
			}
		}
	}
	return false;
}

void Cuboid::restrictMove( Vector3* v, const Cuboid& a ) const {
	//いじっていいコピーを用意
	Cuboid t = *this;
	//移動ベクタを内部単位に変換。
	int x = static_cast< int >( v->x * INTERNAL_UNIT );
	int y = static_cast< int >( v->y * INTERNAL_UNIT );
	int z = static_cast< int >( v->z * INTERNAL_UNIT );
	//まずXYZ全部移動
	t.mX += x;
	t.mY += y;
	t.mZ += z;
	bool r = t.isIntersect( a );
	if ( !r ){
		//あたらない。安心して動け。
		return;
	}
	//当たってしまった。次に
	//xyなら大丈夫、yzなら大丈夫、zxなら大丈夫、の3パターンを試す。
	//xyとzx両方で大丈夫だがyzだけだめ、ということはありうるのだが、
	//扱いようがないので順番に外れたらそれで終わりにする。
	t.mZ -= z;
	r = t.isIntersect( a ); //XY
	if ( !r ){
		v->z = 0.0;
		return;
	}
	t.mZ += z;
	t.mX -= x;
	r = t.isIntersect( a ); //YZ
	if ( !r ){
		v->x = 0.0;
		return;
	}
	t.mX += x;
	t.mY -= y;
	r = t.isIntersect( a ); //ZX
	if ( !r ){
		v->y = 0.0;
		return;
	}
	//後はxyz一方向だけオーケーの場合を探る。
	t.mZ -= z;
	r = t.isIntersect( a ); //X
	if ( !r ){
		v->y = v->z = 0.0;
		return;
	}
	t.mX -= x;
	t.mY += y;
	r = t.isIntersect( a ); //Y
	if ( !r ){
		v->x = v->z = 0.0;
		return;
	}
	t.mY -= y;
	t.mZ += z;
	r = t.isIntersect( a ); //Z
	if ( !r ){
		v->x = v->y = 0.0;
		return;
	}
	//ここまで来てしまった。どう動いても駄目だ。全部捨てる。
	v->set( 0.0, 0.0, 0.0 );
}