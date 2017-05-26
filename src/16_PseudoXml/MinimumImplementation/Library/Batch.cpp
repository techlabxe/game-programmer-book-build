#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "Library/Batch.h"
#include "Library/VertexBuffer.h"
#include "Library/IndexBuffer.h"
#include "Library/GraphicsDatabase.h"
#include "Library/Texture.h"
#include "Library/Matrix44.h"
#include "Library/Vector3.h"
#include "Library/Vector2.h"

#include "PseudoXml/Element.h"
#include "PseudoXml/Attribute.h"
using namespace PseudoXml;
using namespace GameLib;

Batch::Batch( const Element* e, const GraphicsDatabase& db ) : 
mVertexBuffer( 0 ),
mIndexBuffer( 0 ),
mTexture( 0 ),
mBlendMode( Framework::BLEND_OPAQUE ){
	//名前やらなにやらを抜く
	int an = e->attributeNumber();
	for ( int i = 0; i < an; ++i ){
		const Attribute* a = e->attribute( i );
		const string* name = a->name();
		const string* value = a->value();
		if ( *name == "name" ){ 
			mName = *( a->value() );
		}else if ( *name == "vertexBuffer" ){
			mVertexBuffer = db.vertexBuffer( value->c_str() );
		}else if ( *name == "indexBuffer" ){
			mIndexBuffer = db.indexBuffer( value->c_str() );
		}else if ( *name == "texture" ){
			mTexture = db.texture( value->c_str() );
		}else if ( *name == "blend" ){
			if ( *value == "opaque" ){
				mBlendMode = Framework::BLEND_OPAQUE;
			}else if ( *value == "additive" ){
				mBlendMode = Framework::BLEND_ADDITIVE;
			}else if ( *value == "linear" ){
				mBlendMode = Framework::BLEND_LINEAR;
			}
		}
	}
}

Batch::Batch(
const VertexBuffer* vb,
const IndexBuffer* ib,
const ::Texture* tex, //using namespace GameLibのせいでどっちのTextureかわからない
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
	if ( mTexture ){
		mTexture->set();
	}else{
		f.setTexture( 0 );
	}
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

const string* Batch::name() const {
	return &mName;
}
