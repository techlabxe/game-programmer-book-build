#ifndef INCLUDED_TREE_H
#define INCLUDED_TREE_H

namespace GameLib{
	namespace PseudoXml{
		class Element;
	}
}

#include "Library/Vector3.h"
class Matrix44;
class Batch;
class Node;
class TreeTemplate;
class Animation;

class Tree{
public:
	Tree( const TreeTemplate& );
	~Tree();
	void draw( 
		const Matrix44& projectionViewMatrix,
		const Vector3& lightVector,
		const Vector3& lightColor,
		const Vector3& ambient ) const;
	void setPosition( const Vector3& );
	void setAngle( const Vector3& );
	void setScale( const Vector3& );
	const Vector3* position() const;
	const Vector3* angle() const;
	const Vector3* scale() const;
	Node* node( const char* name );
	const Node* node( const char* name ) const;
	void setAnimation( const Animation* );
	void update();
private:
	Vector3 mPosition;
	Vector3 mAngle;
	Vector3 mScale;
	Node* mNodes; //0番が根と決まっているので根を指すポインタは不要
	int mNodeNumber;
	double mTime; //アニメーションの時刻
};

#endif
