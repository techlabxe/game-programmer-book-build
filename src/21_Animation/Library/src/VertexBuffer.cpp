#include "GameLib/GameLib.h"
#include "Library/VertexBuffer.h"
#include "Library/Vector3.h"
#include "Library/Vector2.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
using namespace GameLib::PseudoXml;

VertexBuffer::VertexBuffer( Element& e ) :
mPositions( 0 ),
mUVs( 0 ){
	//エラーチェック全然してません
	mSize = e.childNumber();
	if ( mSize > 0 ){
		mPositions = new Vector3[ mSize ];
		mUVs = new Vector2[ mSize ];
		for ( int i = 0; i < mSize; ++i ){
			Element vertex = e.child( i );
			//アトリビュートから抜き出す
			int an = vertex.attributeNumber();
			mUVs[ i ].set( 0.0, 0.0 ); //念のため初期化 
			for ( int j = 0; j < an; ++j ){
				Attribute a = vertex.attribute( j );
				string name = a.name();
				if ( name == "position" ){
					a.getDoubleValues( &mPositions[ i ].x, 3 );
				}else if ( name == "uv" ){
					a.getDoubleValues( &mUVs[ i ].x, 2 );
				}
			}
		}
	}
	//名前を抜く
	int an = e.attributeNumber();
	for ( int i = 0; i < an; ++i ){
		Attribute a = e.attribute( i );
		string name = a.name();
		if ( name == "name" ){ 
			mName = a.value();
		}
	}
}

VertexBuffer::VertexBuffer( int size ) :
mPositions( 0 ),
mUVs( 0 ),
mSize( size ){
	mPositions = new Vector3[ mSize ];
	mUVs = new Vector2[ mSize ];
}

VertexBuffer::~VertexBuffer(){
	SAFE_DELETE_ARRAY( mPositions );
	SAFE_DELETE_ARRAY( mUVs );
}

const Vector3* VertexBuffer::position( int i ) const {
	ASSERT( i >= 0 && i < mSize );
	return &mPositions[ i ];
}

const Vector2* VertexBuffer::uv( int i ) const {
	ASSERT( i >= 0 && i < mSize );
	return &mUVs[ i ];
}

void VertexBuffer::setPosition( int i, const Vector3& p ){
	ASSERT( i >= 0 && i < mSize );
	mPositions[ i ] = p;
}

void VertexBuffer::setUV( int i, const Vector2& uv ){
	ASSERT( i >= 0 && i < mSize );
	mUVs[ i ] = uv;
}

int VertexBuffer::size() const {
	return mSize;
}

const string* VertexBuffer::name() const {
	return &mName;
}
