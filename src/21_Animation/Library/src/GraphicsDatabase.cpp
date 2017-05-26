#include "GameLib/GameLib.h"
#include "Library/GraphicsDatabase.h"
#include "Library/VertexBuffer.h"
#include "Library/IndexBuffer.h"
#include "Library/Texture.h"
#include "Library/Batch.h"
#include "Library/Model.h"
#include "Library/TreeTemplate.h"
#include "Library/Tree.h"
#include "Library/Animation.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
#include "GameLib/PseudoXml/Document.h"
using namespace GameLib::PseudoXml; //かぶってないしusingしちゃえ

GraphicsDatabase::GraphicsDatabase( Element& e ) : 
mVertexBuffers( 0 ),
mIndexBuffers( 0 ),
mTextures( 0 ),
mBatches( 0 ),
mTrees( 0 ),
mAnimations( 0 ),
mVertexBufferNumber( 0 ),
mIndexBufferNumber( 0 ),
mTextureNumber( 0 ),
mBatchNumber( 0 ),
mTreeNumber( 0 ),
mAnimationNumber( 0 ){
	createFromElement( e );
}

GraphicsDatabase::GraphicsDatabase( const char* filename ) : 
mVertexBuffers( 0 ),
mIndexBuffers( 0 ),
mTextures( 0 ),
mBatches( 0 ),
mTrees( 0 ),
mAnimations( 0 ),
mVertexBufferNumber( 0 ),
mIndexBufferNumber( 0 ),
mTextureNumber( 0 ),
mBatchNumber( 0 ),
mTreeNumber( 0 ),
mAnimationNumber( 0 ){
	Document document = Document::create( filename );
	while ( !document.isReady() ){
		; //ロード待ち
	}
	Element root = document.root();
	createFromElement( root );
}

GraphicsDatabase::~GraphicsDatabase(){
	//依存しているものから消すので、最初はTree
	if ( mTrees ){
		for ( int i = 0; i < mTreeNumber; ++i ){
			SAFE_DELETE( mTrees[ i ] );
		}
		SAFE_DELETE_ARRAY( mTrees );
	}

	//次バッチ。
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
	if ( mAnimations ){
		for ( int i = 0; i < mAnimationNumber; ++i ){
			SAFE_DELETE( mAnimations[ i ] );
		}
		SAFE_DELETE_ARRAY( mAnimations );
	}
}

Tree* GraphicsDatabase::createTree( const char* name ) const {
	for ( int i = 0; i < mTreeNumber; ++i ){
		if ( *( mTrees[ i ]->name() ) == name ){
			return new Tree( *mTrees[ i ] );
		}
	}
	return 0;
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
		if ( * ( mVertexBuffers[ i ]->name() ) == name ){
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

const Batch* GraphicsDatabase::batch( const char* name ) const {
	for ( int i = 0; i < mBatchNumber; ++i ){
		if ( *( mBatches[ i ]->name() ) == name ){
			return mBatches[ i ];
		}
	}
	return 0;
}

const TreeTemplate* GraphicsDatabase::tree( const char* name ) const {
	for ( int i = 0; i < mTreeNumber; ++i ){
		if ( *( mTrees[ i ]->name() ) == name ){
			return mTrees[ i ];
		}
	}
	return 0;
}

const Animation* GraphicsDatabase::animation( const char* name ) const {
	for ( int i = 0; i < mAnimationNumber; ++i ){
		if ( *( mAnimations[ i ]->name() ) == name ){
			return mAnimations[ i ];
		}
	}
	return 0;
}

void GraphicsDatabase::createFromElement( Element& e ){
	int n = e.childNumber();
	//まず数を数える
	for ( int i = 0; i < n; ++i ){
		Element child = e.child( i );
		string name = child.name();
		if ( name == "VertexBuffer" ){
			++mVertexBufferNumber;
		}else if ( name == "IndexBuffer" ){
			++mIndexBufferNumber;
		}else if ( name == "Texture" ){
			++mTextureNumber;
		}else if ( name == "Batch" ){
			++mBatchNumber;
		}else if ( name == "Tree" ){
			++mTreeNumber;
		}else if ( name == "Animation" ){
			++mAnimationNumber;
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
	if ( mTreeNumber > 0 ){
		mTrees = new TreeTemplate*[ mTreeNumber ];
	}
	if ( mAnimationNumber > 0 ){
		mAnimations = new Animation*[ mAnimationNumber ];
	}
	//今何個目に書き込んでいるか
	int vertexBufferPos = 0;
	int indexBufferPos = 0;
	int texturePos = 0;
	int batchPos = 0;
	int treePos = 0;
	int animationPos = 0;

	//何にも依存していないものを作る
	for ( int i = 0; i < n; ++i ){
		Element child = e.child( i );
		string name = child.name();
		if ( name == "VertexBuffer" ){
			mVertexBuffers[ vertexBufferPos ] = new VertexBuffer( child );
			++vertexBufferPos;
		}else if ( name == "IndexBuffer" ){
			mIndexBuffers[ indexBufferPos ] = new IndexBuffer( child );
			++indexBufferPos;
		}else if ( name == "Texture" ){
			mTextures[ texturePos ] = new Texture( child );
			++texturePos;
		}else if ( name == "Animation" ){
			mAnimations[ animationPos ] = new Animation( child );
			++animationPos;
		}
	}
	//BatchはIndexBuffer,VertexBuffer,Textureに依存しているので後でやる
	for ( int i = 0; i < n; ++i ){
		Element child = e.child( i );
		string name = child.name();
		if ( name == "Batch" ){
			mBatches[ batchPos ] = new Batch( child, *this );
			++batchPos;
		}
	}
	//TreeはBatchに依存しているのでさらに後でやる
	for ( int i = 0; i < n; ++i ){
		Element child = e.child( i );
		string name = child.name();
		if ( name == "Tree" ){
			mTrees[ treePos ] = new TreeTemplate( child, *this );
			++treePos;
		}
	}
}

