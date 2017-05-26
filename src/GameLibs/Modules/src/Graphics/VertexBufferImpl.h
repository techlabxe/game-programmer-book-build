#ifndef INCLUDED_GAMELIB_GRAPHICS_VERTEXBUFFERIMPL_H
#define INCLUDED_GAMELIB_GRAPHICS_VERTEXBUFFERIMPL_H

#include "GameLib/Graphics/Vertex.h"
#include "GameLib/Base/Impl/ReferenceType.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
#include "GameLib/Base/RefString.h"

namespace GameLib{
using namespace GameLib::PseudoXml;
namespace Graphics{

class VertexBuffer::Impl : public ReferenceType{
public:
	Impl( ConstElement e, IDirect3DDevice9* device ) : 
	mDxObject( 0 ),
	mVertexNumber( 0 ),
	mName( 0 ){
		//エラーチェック全然してません
		mVertexNumber = e.childNumber();
		if ( mVertexNumber > 0 ){
			createDxObject( device );
			Vertex* vertices = lock();
			for ( int i = 0; i < mVertexNumber; ++i ){
				ConstElement vertex = e.child( i );
				//アトリビュートから抜き出す
				int an = vertex.attributeNumber();
				//デフォルト値充填
				Vertex& v = vertices[ i ];
				v.mPosition.w = 1.f;
				v.mNormal.set( 0.f, 0.f, 0.f );
				v.mColor = 0xffffffff;
				v.mUv.set( 0.f, 0.f );
				for ( int j = 0; j < an; ++j ){
					ConstAttribute a = vertex.attribute( j );
					if ( RefString( "position" ) == a.name() ){
						a.getFloatValues( &v.mPosition.x, 3 );
					}else if ( RefString( "normal" ) == a.name() ){
						a.getFloatValues( &v.mNormal.x, 3 );
					}else if ( RefString( "uv" ) == a.name() ){
						a.getFloatValues( &v.mUv.x, 2 );
					}else if ( RefString( "color" ) == a.name() ){
						//RGBA32形式カラーに変換
						float color[ 4 ];
						a.getFloatValues( color, 4 );
						unsigned c[ 4 ];
						for ( int i = 0; i < 4; ++i ){
							c[ i ] = static_cast< unsigned >( color[ i ] * 255.f + 0.5f );
							c[ i ] = ( c[ i ] < 0.f ) ? 0 : c[ i ];
							c[ i ] = ( c[ i ] > 255 ) ? 255 : c[ i ];
						}
						//RGBAの順でファイルに書かれているわけだが、32bit形式だとARGBになる。注意。
						v.mColor = ( c[ 3 ] << 24 ) | ( c[ 0 ] << 16 ) | ( c[ 1 ] << 8 ) | c[ 2 ];
					}
				}
			}
			unlock();
			vertices = 0;
		}
		//名前を抜く
		int an = e.attributeNumber();
		for ( int i = 0; i < an; ++i ){
			ConstAttribute a = e.attribute( i );
			if ( RefString( "name" ) == a.name() ){ 
				RefString name( a.value() );
				mName = name.copyOriginal();
			}
		}
	}
	Impl( int vertexNumber, IDirect3DDevice9* device ) : 
	mDxObject( 0 ),
	mVertexNumber( vertexNumber ),
	mName( 0 ){
		createDxObject( device );
	}
	~Impl(){
		mDxObject->Release();
		mDxObject = 0;
		SAFE_DELETE_ARRAY( mName );
	}
	void createDxObject( IDirect3DDevice9* device ){
		HRESULT hr;
		hr = device->CreateVertexBuffer( 
			mVertexNumber * sizeof( Vertex ), 
			0,
			0,
			D3DPOOL_MANAGED, 
			&mDxObject, 
			NULL );
		if ( FAILED( hr ) ){
			ASSERT( hr != D3DERR_INVALIDCALL && "CreateVertexBuffer : INVALID CALL" );
			ASSERT( hr != D3DERR_OUTOFVIDEOMEMORY && "CreateVertexBuffer : OUT OF VIDEO MEMORY" );
			ASSERT( hr != E_OUTOFMEMORY && "CreateVertexBuffer : E OUT OF MEMORY" );
			ASSERT( 0 && "CreateVertexBuffer : unknown error" );
		}
	}
	Vertex* lock(){
		void* r = 0;
		mDxObject->Lock( 0, 0, &r, 0 );
		return static_cast< Vertex* >( r );
	}
	void unlock(){
		mDxObject->Unlock();
	}
	IDirect3DVertexBuffer9* mDxObject;
	int mVertexNumber;
	char* mName;
private:
	void operator=( const Impl& ); //禁止
};

} //namespace Graphics
} //namespace GameLib

#endif
