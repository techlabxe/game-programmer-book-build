#ifndef INCLUDED_GAMELIB_GRAPHICS_INDEXBUFFERIMPL_H
#define INCLUDED_GAMELIB_GRAPHICS_INDEXBUFFERIMPL_H

#include "GameLib/Base/Impl/ReferenceType.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
#include "GameLib/Base/RefString.h"

namespace GameLib{
using namespace GameLib::PseudoXml;
namespace Graphics{

class IndexBuffer::Impl : public ReferenceType{
public:
	Impl( ConstElement e, IDirect3DDevice9* device ) :
	mDxObject( 0 ),
	mIndexNumber( 0 ),
	mName( 0 ){
		int n = e.childNumber();
		mIndexNumber = n * 3; //三角形数×3
		if ( mIndexNumber > 0 ){
			createDxObject( device );
			unsigned short* indices = lock();
			for ( int i = 0; i < n; ++i ){
				ConstElement triangle = e.child( i );
				ASSERT( triangle.attributeNumber() > 0 && "IndexBuffer : It's not valid element." );
				ConstAttribute a = triangle.attribute( 0 ); //indices一個しかないので決めうち
				//unsigned shortを取る関数がないのでintに入れてからコピー
				int tmp[ 3 ];
				int count = a.getIntValues( tmp, 3 );
				ASSERT( count == 3 );
				for ( int j = 0; j < 3; ++j ){
					ASSERT( tmp[ j ] >= 0 && tmp[ j ] <= 0xffff );
					indices[ i * 3 + j ] = static_cast< unsigned short >( tmp[ j ] );
				}
			}
			unlock();
			indices = 0;
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
	Impl( int indexNumber, IDirect3DDevice9* device ) : 
	mDxObject( 0 ),
	mIndexNumber( indexNumber ),
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
		hr = device->CreateIndexBuffer( 
			mIndexNumber * sizeof( DWORD ), 
			0,
			D3DFMT_INDEX16, 
			D3DPOOL_MANAGED, 
			&mDxObject, 
			NULL );
		if ( FAILED( hr ) ){
			ASSERT( hr != D3DERR_INVALIDCALL && "CreateIndexBuffer : INVALID CALL" );
			ASSERT( hr != D3DERR_OUTOFVIDEOMEMORY && "CreateIndexBuffer : OUT OF VIDEO MEMORY" );
			ASSERT( hr != E_OUTOFMEMORY && "CreateIndexBuffer : E OUT OF MEMORY" );
			ASSERT( 0 && "CreateIndexBuffer : unknown error" );
		}
	}
	unsigned short* lock(){
		void* r = 0;
		mDxObject->Lock( 0, 0, &r, 0 );
		return static_cast< unsigned short* >( r );
	}
	void unlock(){
		mDxObject->Unlock();
	}
	const char* name() const {
		return mName;
	}
	IDirect3DIndexBuffer9* mDxObject;
	int mIndexNumber;
	char* mName;
private:
	void operator=( const Impl& ); //禁止
};

} //namespace Graphics
} //namespace GameLib

#endif
