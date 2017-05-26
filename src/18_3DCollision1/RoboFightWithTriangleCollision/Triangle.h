#ifndef INCLUDED_TRIANGLE_H
#define INCLUDED_TRIANGLE_H

#include "Library/Vector3.h"

class Triangle{
public:
	//始点と線分ベクタ
	bool isIntersect( const Vector3& lineBegin, const Vector3& lineVector ) const;
	//受け取った移動ベクタの成分のうち相手に当たらないで動ける成分だけを残す
	void restrictMove( Vector3* moveVectorInOut, const Vector3& position ) const;
	//位置設定
	void setPosition( const Vector3& p0, const Vector3& p1, const Vector3& p2 );
private:
	Vector3 mPosition[ 3 ]; //3頂点
};

#endif

