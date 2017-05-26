#ifndef INCLUDED_NODE_H
#define INCLUDED_NODE_H

#include <string>
using namespace std;

#include "Library/Vector3.h"
class Matrix44;
class Matrix34;
class Batch;
class NodeTemplate;
class TreeTemplate;
class AnimationNode;

class Node{
public:
	Node(); //デフォルトコンストラクタ
	Node( const Batch*, int childNumber );
	~Node();
	void draw( 
		const Matrix44& projectionViewMatrix,
		const Matrix34& parentWorldMatrix,
		const Vector3& lightVector,
		const Vector3& lightColor,
		const Vector3& ambient ) const;
	void setTranslation( const Vector3& );
	void setRotation( const Vector3& );
	void setScale( const Vector3& );
	const Batch* batch() const;
	const Vector3* translation() const;
	const Vector3* rotation() const;
	const Vector3* scale() const;
	void setColor( const Vector3& );
	void setChild( int index, Node* );
	void setChildNumber( int );
	void setBatch( const Batch* );
	const string* name() const;
	void setName( const char* );
	void setAnimation( const AnimationNode* );
	void update( double time );
private:
	Vector3 mTranslation;
	Vector3 mRotation;
	Vector3 mScale;
	Vector3 mColor;
	const Batch* mBatch;
	Node** mChildren;
	int mChildNumber;
	string mName;
	const AnimationNode* mAnimation; //アニメーション
};

#endif
