#include "Triangle.h"
#include "Library/Vector3.h"
#include <limits>
using namespace std;


void Triangle::setPosition( const Vector3& p0, const Vector3& p1, const Vector3& p2 ){
	mPosition[ 0 ] = p0;
	mPosition[ 1 ] = p1;
	mPosition[ 2 ] = p2;
}

double Triangle::getIntersectionTime( const Vector3& lineBegin, const Vector3& lineVector ) const {
	double inf = numeric_limits< double >::max(); //doubleで表せる一番大きな数。当たらなければこれが返る
	//本と同じ記号に置きなおす
	const Vector3& a = lineBegin;
	const Vector3& b = lineVector;
	const Vector3& c = mPosition[ 0 ];
	Vector3 d, e;
	d.setSub( mPosition[ 1 ], c );
	e.setSub( mPosition[ 2 ], c );
	Vector3 f;
	f.setSub( c, a );
	//法線を求めよう。
	Vector3 n;
	n.setCross( d, e );
	//tを求める。
	double nf = n.dot( f );
	double nb = n.dot( b );
	if ( nb == 0.f ){ //平行
	  return inf; //はずれにしちゃえ。
	}
	double t = nf / nb;
	if ( t < 0 || t > 1 ){ 
	  return inf;
	}
	///vを求める。
	Vector3 p;
	p.setMadd( b, t, a );
	Vector3 g;
	g.setSub( p, c );
	double gd = g.dot( d );
	double ge = g.dot( e );
	double dd = d.dot( d );
	double ee = e.dot( e );
	double de = d.dot( e );
	//de*de - dd*eeの逆数は二回使うので、一回で済ませて高速化する。
	double dedeEedd = de * de - ee * dd; //こいつは0にはなりえないのでチェックなしで逆数が取れる
	double rcpDedeEedd = 1.0 / dedeEedd; 

	double u = ( gd*de - ge*dd ) * rcpDedeEedd;
	if ( u < 0 || u > 1 ){
	  return inf;
	}
	double v = ( ge*de - gd*ee ) * rcpDedeEedd;
	if ( v < 0 || ( u + v > 1 ) ){
	  return inf;
	}
	return t; //tが時刻
}

bool Triangle::isIntersect( const Vector3& lineBegin, const Vector3& lineVector ) const {
	double t = getIntersectionTime( lineBegin, lineVector );
	return ( t >= 0.0 && t <= 1.0 );
}

bool Triangle::restrictMove( Vector3* v, const Vector3& p ) const {
	//まず判定
	bool r = isIntersect( p, *v );
	if ( !r ){
		return false; //当たらないのでそのまま終わる
	}
	//さて当たっているならば、ベクタを修正する。
	//式は法線nを使って以下のように書ける
	//a = v - dot( n, v ) * n / |n|^2
	Vector3 d, e, n;
	d.setSub( mPosition[ 1 ], mPosition[ 0 ] );
	e.setSub( mPosition[ 2 ], mPosition[ 0 ] );
	n.setCross( d, e );

	n *= n.dot( *v ) / n.squareLength();
	*v -= n;
	return true;
}

void Triangle::modifyMove( Vector3* v ) const {
	Vector3 d, e, n;
	d.setSub( mPosition[ 1 ], mPosition[ 0 ] );
	e.setSub( mPosition[ 2 ], mPosition[ 0 ] );
	n.setCross( d, e );

	n *= n.dot( *v ) / n.squareLength();
	*v -= n;
}
