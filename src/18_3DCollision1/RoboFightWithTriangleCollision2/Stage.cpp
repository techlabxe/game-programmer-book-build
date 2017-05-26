#include "GameLib/Framework.h"

#include "Stage.h"
#include "Triangle.h"
#include "Library/GraphicsDatabase.h"
#include "Library/Batch.h"
#include "Library/IndexBuffer.h"
#include "Library/VertexBuffer.h"
#include "Library/Model.h"
#include "Library/Matrix44.h"
#include "Library/Vector3.h"
#include <limits>
using namespace std;

Stage::Stage() : mDatabase( 0 ), mModel( 0 ), mTriangles( 0 ){
	mDatabase = new GraphicsDatabase( "stage.txt" );
	mModel = mDatabase->createModel( "draw" );

	//衝突処理用データ作成
	const Batch* batch = mDatabase->batch( "collision" );
	const IndexBuffer* ib = batch->indexBuffer();
	const VertexBuffer* vb = batch->vertexBuffer();

	mTriangleNumber = ib->size() / 3;
	mTriangles = new Triangle[ mTriangleNumber ];
	for ( int i = 0; i < mTriangleNumber; ++i ){
		unsigned i0 = ib->index( i * 3 + 0 );
		unsigned i1 = ib->index( i * 3 + 1 );
		unsigned i2 = ib->index( i * 3 + 2 );
		mTriangles[ i ].setPosition( 
			*vb->position( i0 ),
			*vb->position( i1 ),
			*vb->position( i2 ) );
	}
}

Stage::~Stage(){
	SAFE_DELETE( mModel );
	SAFE_DELETE( mDatabase );
	SAFE_DELETE_ARRAY( mTriangles );
}

void Stage::draw( const Matrix44& pvm ) const {
	mModel->draw( pvm );
}

void Stage::restrictMove( Vector3* v, const Vector3& p ) const {
	//一周目は普通に。ただし移動量はコピーをとる。
	Vector3 vCopy = *v;
	bool hit = false;
	for ( int i = 0; i < mTriangleNumber; ++i ){
		hit = mTriangles[ i ].restrictMove( &vCopy, p );
		if ( hit ){
			break;
		}
	}
	//当たってない。そのまま
	if ( !hit ){
		return;
	}
	//当たった場合、修正後のベクタで二周目のループを回す。
	//当たるなら修正移動は捨てる
	hit = false;
	for ( int i = 0; i < mTriangleNumber; ++i ){
		hit = mTriangles[ i ].isIntersect( p, vCopy );
		if ( hit ){
			break;
		}
	}
	if ( hit ){
		v->set( 0.0, 0.0, 0.0 );
	}else{
		*v = vCopy;
	}
}
