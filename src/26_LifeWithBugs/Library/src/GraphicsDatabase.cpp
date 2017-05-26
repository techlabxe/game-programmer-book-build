#include "GameLib/GameLib.h"
#include "Library/GraphicsDatabase.h"
#include "Library/VertexBuffer.h"
#include "Library/IndexBuffer.h"
#include "Library/Texture.h"
#include "Library/Batch.h"
#include "Library/Model.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
#include "GameLib/PseudoXml/Document.h"
using namespace GameLib::PseudoXml; //かぶってないしusingしちゃえ

GraphicsDatabase::GraphicsDatabase( Element& e ){
	createFromElement( e );
}

GraphicsDatabase::GraphicsDatabase( const char* filename ){
	Document document = Document::create( filename );
	while ( !document.isReady() ){
		; //ロード待ち
	}
	Element root = document.root();
	createFromElement( root );
}

GraphicsDatabase::~GraphicsDatabase(){
	//何もいらない。
}

Model* GraphicsDatabase::createModel( const char* name ) const {
	for ( int i = 0; i < mBatches.size(); ++i ){
		if ( *( mBatches[ i ]->name() ) == name ){
			return NEW Model( mBatches[ i ] );
		}
	}
	return 0;
}

ConstSharedPtr< VertexBuffer > GraphicsDatabase::getVertexBuffer( const char* name ) const {
	for ( int i = 0; i < mVertexBuffers.size(); ++i ){
		if ( * ( mVertexBuffers[ i ]->name() ) == name ){
			return mVertexBuffers[ i ];
		}
	}
	return 0;
}

ConstSharedPtr< IndexBuffer > GraphicsDatabase::getIndexBuffer( const char* name ) const {
	for ( int i = 0; i < mIndexBuffers.size(); ++i ){
		if ( *( mIndexBuffers[ i ]->name() ) == name ){
			return mIndexBuffers[ i ];
		}
	}
	return 0;
}

ConstSharedPtr< Texture > GraphicsDatabase::getTexture( const char* name ) const {
	for ( int i = 0; i < mTextures.size(); ++i ){
		if ( *( mTextures[ i ]->name() ) == name ){
			return mTextures[ i ];
		}
	}
	return 0;
}

ConstSharedPtr< Batch > GraphicsDatabase::getBatch( const char* name ) const {
	for ( int i = 0; i < mBatches.size(); ++i ){
		if ( *( mBatches[ i ]->name() ) == name ){
			return mBatches[ i ];
		}
	}
	return 0;
}

void GraphicsDatabase::createFromElement( Element& e ){
	int n = e.childNumber();
	//まず数を数える
	int vertexBufferNumber = 0;
	int indexBufferNumber = 0;
	int textureNumber = 0;
	int batchNumber = 0;
	for ( int i = 0; i < n; ++i ){
		Element child = e.child( i );
		string name = child.name();
		if ( name == "VertexBuffer" ){
			++vertexBufferNumber;
		}else if ( name == "IndexBuffer" ){
			++indexBufferNumber;
		}else if ( name == "Texture" ){
			++textureNumber;
		}else if ( name == "Batch" ){
			++batchNumber;
		}
	}
	//配列確保
	mVertexBuffers.setSize( vertexBufferNumber );
	mIndexBuffers.setSize( indexBufferNumber );
	mTextures.setSize( textureNumber );
	mBatches.setSize( batchNumber );
	//今何個目に書き込んでいるか
	int vertexBufferPos = 0;
	int indexBufferPos = 0;
	int texturePos = 0;
	int batchPos = 0;

	//何にも依存していない三つを作る
	for ( int i = 0; i < n; ++i ){
		Element child = e.child( i );
		string name = child.name();
		if ( name == "VertexBuffer" ){
			mVertexBuffers[ vertexBufferPos ] = NEW VertexBuffer( child );
			++vertexBufferPos;
		}else if ( name == "IndexBuffer" ){
			mIndexBuffers[ indexBufferPos ] = NEW IndexBuffer( child );
			++indexBufferPos;
		}else if ( name == "Texture" ){
			mTextures[ texturePos ] = NEW Texture( child );
			++texturePos;
		}
	}
	//Batchは他に依存しているので後でやる
	for ( int i = 0; i < n; ++i ){
		Element child = e.child( i );
		string name = child.name();
		if ( name == "Batch" ){
			mBatches[ batchPos ] = NEW Batch( child, *this );
			++batchPos;
		}
	}
}

