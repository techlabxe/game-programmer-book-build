#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
using namespace GameLib;
#include "Library/Batch.h"
#include "Library/VertexBuffer.h"
#include "Library/IndexBuffer.h"
#include "Library/Matrix44.h"
#include "Library/Vector3.h"
#include "Library/Vector2.h"

Batch::Batch(
const VertexBuffer* vb,
const IndexBuffer* ib,
const Texture* tex,
Framework::BlendMode blend ) :
mVertexBuffer( vb ),
mIndexBuffer( ib ),
mTexture( tex ),
mBlendMode( blend ){
}

Batch::~Batch(){
	mVertexBuffer = 0;
	mIndexBuffer = 0;
	mTexture = 0;
}

void Batch::draw( const Matrix44& transform ) const {
	Framework f = Framework::instance();
	//テクスチャセット
	f.setTexture( mTexture );
	//ブレンドモードセット
	f.setBlendMode( mBlendMode );
	//ブレンドモードによってZバッファ書き込みのフラグをOn,Off
	if ( mBlendMode == Framework::BLEND_OPAQUE ){
		f.enableDepthWrite( true );
	}else{
		f.enableDepthWrite( false );
	}
	//ZテストはいつもOn
	f.enableDepthTest( true );
	//頂点変換
	int vertexNumber = mVertexBuffer->size();
	double* p4 = new double[ vertexNumber * 4 ];
	for ( int i = 0;i < vertexNumber; ++i ){
		transform.multiply( &p4[ i * 4 ], *mVertexBuffer->position( i ) );
	}
	int triangleNumber = mIndexBuffer->size() / 3;
	for ( int i = 0; i < triangleNumber; ++i ){
		unsigned i0 = mIndexBuffer->index( i * 3 + 0 );
		unsigned i1 = mIndexBuffer->index( i * 3 + 1 );
		unsigned i2 = mIndexBuffer->index( i * 3 + 2 );
		f.drawTriangle3DH(
			&p4[ i0 * 4 ],
			&p4[ i1 * 4 ],
			&p4[ i2 * 4 ],
			&mVertexBuffer->uv( i0 )->x,
			&mVertexBuffer->uv( i1 )->x,
			&mVertexBuffer->uv( i2 )->x );
	}
	SAFE_DELETE_ARRAY( p4 );
}