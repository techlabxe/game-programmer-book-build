#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "Library/GraphicsDatabase.h"
#include "Library/VertexBuffer.h"
#include "Library/IndexBuffer.h"
#include "Library/Batch.h"
#include "Library/Texture.h"
#include "Library/Model.h"
#include "PseudoXml/Element.h"
#include "PseudoXml/Attribute.h"
#include "PseudoXml/Document.h"
using namespace PseudoXml; //かぶってないしusingしちゃえ

GraphicsDatabase::GraphicsDatabase( const Element* e ) : 
mVertexBuffers( 0 ),
mIndexBuffers( 0 ),
mTextures( 0 ),
mBatches( 0 ),
mVertexBufferNumber( 0 ),
mIndexBufferNumber( 0 ),
mTextureNumber( 0 ),
mBatchNumber( 0 ){
	createFromElement( e );
}

GraphicsDatabase::GraphicsDatabase( const char* filename ) : 
mVertexBuffers( 0 ),
mIndexBuffers( 0 ),
mTextures( 0 ),
mBatches( 0 ),
mVertexBufferNumber( 0 ),
mIndexBufferNumber( 0 ),
mTextureNumber( 0 ),
mBatchNumber( 0 ){
	Document document( filename );
	createFromElement( document.root() );
}

GraphicsDatabase::~GraphicsDatabase(){
	//まずバッチから。
	if ( mBatches ){
		for ( int i = 0; i < mBatchNumber; ++i ){
			SAFE_DELETE( mBatches[ i ] );
		}
		SAFE_DELETE_ARRAY( mBatches );
	}
	//テクスチャ
	if ( mTextures ){
		for ( int i = 0; i < mTextureNumber; ++i ){
			SAFE_DELETE( mTextures[ i ] );
		}
		SAFE_DELETE_ARRAY( mTextures );
	}
	//インデクスバッファ
	if ( mIndexBuffers ){
		for ( int i = 0; i < mIndexBufferNumber; ++i ){
			SAFE_DELETE( mIndexBuffers[ i ] );
		}
		SAFE_DELETE_ARRAY( mIndexBuffers );
	}
	//頂点バッファ
	if ( mVertexBuffers ){
		for ( int i = 0; i < mVertexBufferNumber; ++i ){
			SAFE_DELETE( mVertexBuffers[ i ] );
		}
		SAFE_DELETE_ARRAY( mVertexBuffers );
	}
}

Model* GraphicsDatabase::createModel( const char* name ) const {
	for ( int i = 0; i < mBatchNumber; ++i ){
		if ( *( mBatches[ i ]->name() ) == name ){
			return new Model( mBatches[ i ] );
		}
	}
	return 0;
}

const VertexBuffer* GraphicsDatabase::vertexBuffer( const char* name ) const {
	for ( int i = 0; i < mVertexBufferNumber; ++i ){
		if ( *( mVertexBuffers[ i ]->name() ) == name ){
			return mVertexBuffers[ i ];
		}
	}
	return 0;
}

const IndexBuffer* GraphicsDatabase::indexBuffer( const char* name ) const {
	for ( int i = 0; i < mIndexBufferNumber; ++i ){
		if ( *( mIndexBuffers[ i ]->name() ) == name ){
			return mIndexBuffers[ i ];
		}
	}
	return 0;
}

const Texture* GraphicsDatabase::texture( const char* name ) const {
	for ( int i = 0; i < mTextureNumber; ++i ){
		if ( *( mTextures[ i ]->name() ) == name ){
			return mTextures[ i ];
		}
	}
	return 0;
}

void GraphicsDatabase::createFromElement( const Element* e ){
	int n = e->childNumber();
	//まず数を数える
	for ( int i = 0; i < n; ++i ){
		const Element* child = e->child( i );
		const string* name = child->name();
		if ( *name == "VertexBuffer" ){
			++mVertexBufferNumber;
		}else if ( *name == "IndexBuffer" ){
			++mIndexBufferNumber;
		}else if ( *name == "Texture" ){
			++mTextureNumber;
		}else if ( *name == "Batch" ){
			++mBatchNumber;
		}
	}
	//配列確保
	if ( mVertexBufferNumber > 0 ){
		mVertexBuffers = new VertexBuffer*[ mVertexBufferNumber ];
	}
	if ( mIndexBufferNumber > 0 ){
		mIndexBuffers = new IndexBuffer*[ mIndexBufferNumber ];
	}
	if ( mTextureNumber > 0 ){
		mTextures = new Texture*[ mTextureNumber ];
	}
	if ( mBatchNumber > 0 ){
		mBatches = new Batch*[ mBatchNumber ];
	}
	//今何個目に書き込んでいるか
	int vertexBufferPos = 0;
	int indexBufferPos = 0;
	int texturePos = 0;
	int batchPos = 0;

	//何にも依存していない三つを作る
	for ( int i = 0; i < n; ++i ){
		const Element* child = e->child( i );
		const string* name = child->name();
		if ( *name == "VertexBuffer" ){
			mVertexBuffers[ vertexBufferPos ] = new VertexBuffer( child );
			++vertexBufferPos;
		}else if ( *name == "IndexBuffer" ){
			mIndexBuffers[ indexBufferPos ] = new IndexBuffer( child );
			++indexBufferPos;
		}else if ( *name == "Texture" ){
			mTextures[ texturePos ] = new Texture( child );
			++texturePos;
		}
	}
	//Batchは他に依存しているので後でやる
	for ( int i = 0; i < n; ++i ){
		const Element* child = e->child( i );
		const string* name = child->name();
		if ( *name == "Batch" ){
			mBatches[ batchPos ] = new Batch( child, *this );
			++batchPos;
		}
	}
}

