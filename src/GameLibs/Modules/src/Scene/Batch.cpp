#include "GameLib/GameLib.h"
#include "GameLib/Scene/Batch.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
#include "GameLib/Base/RefString.h"
#include "GameLib/Graphics/Enum.h"

#include "Scene/BatchImpl.h"
#include "Scene/ContainerImpl.h"

namespace GameLib{
namespace Scene{
using namespace GameLib::Graphics;

//BatchImpl.hに置きたいが、コンテナにアクセスする関係でおけず、ここに置く。
Batch::Impl::Impl( PseudoXml::ConstElement& e, Container::Impl& c ) : 
mDiffuseColor( 1.f, 1.f, 1.f ),
mSpecularColor( 1.f, 1.f, 1.f ),
mEmissionColor( 0.f, 0.f, 0.f ),
mSpecularSharpness( 1.f ),
mTransparency( 1.f ),
mBufferOffset( 0 ),
mPrimitiveNumber( 0 ),
mBlendMode( BLEND_OPAQUE ),
mCullMode( CULL_MAX ),
mPrimitiveType( PRIMITIVE_TRIANGLE ),
mLighting( true ),
mName( 0 ),
mContainer( &c ){
	//名前やらなにやらを抜く
	int an = e.attributeNumber();
	for ( int i = 0; i < an; ++i ){
		ConstAttribute a = e.attribute( i );
		RefString name( a.name() );
		if ( name == "name" ){
			RefString value( a.value() );
			mName = c.allocateString( value );
		}else if ( name == "vertexBuffer" ){
			mVertexBuffer = c.vertexBuffer( a.value() );
			ASSERT( mVertexBuffer && "Batch : VertexBuffer not found!" );
		}else if ( name == "indexBuffer" ){
			mIndexBuffer = c.indexBuffer( a.value() );
			ASSERT( mIndexBuffer && "Batch : IndexBuffer not found!" );
		}else if ( name == "texture" ){
			mTexture = c.texture( a.value() );
			ASSERT( mIndexBuffer && "Batch : Texture not found!" );
		}else if ( name == "blend" ){
			RefString value( a.value() );
			if ( value == "opaque" ){
				mBlendMode = BLEND_OPAQUE;
			}else if ( value == "additive" ){
				mBlendMode = BLEND_ADDITIVE;
			}else if ( value == "linear" ){
				mBlendMode = BLEND_LINEAR;
			}
		}else if ( name == "cull" ){
			RefString value( a.value() );
			if ( value == "none" ){
				mCullMode = CULL_NONE;
			}else if ( value == "back" ){
				mCullMode = CULL_BACK;
			}else if ( value == "front" ){
				mCullMode = CULL_FRONT;
			}
		}else if ( name == "bufferOffset" ){
			mBufferOffset = a.getIntValue();
		}else if ( name == "primitiveNumber" ){
			mPrimitiveNumber = a.getIntValue();
		}else if ( name == "diffuseColor" ){
			a.getFloatValues( &mDiffuseColor.x, 3 );
		}else if ( name == "specularColor" ){
			a.getFloatValues( &mSpecularColor.x, 3 );
		}else if ( name == "emissionColor" ){
			a.getFloatValues( &mEmissionColor.x, 3 );
		}else if ( name == "specularSharpness" ){
			mSpecularSharpness = a.getFloatValue();
		}else if ( name == "transparency" ){
			mTransparency = a.getFloatValue();
		}else if ( name == "lighting" ){
			if ( RefString( "none" ) == a.value() ){
				mLighting = false;
			}
		}
	}
	//透明度設定がされているならリニアーに上書き
	if ( mTransparency < 1.f && mBlendMode == BLEND_OPAQUE ){
		mBlendMode = BLEND_LINEAR;
	}
	ASSERT( mVertexBuffer && "Batch : VertexBuffer not specified!" );
	if ( mPrimitiveNumber == 0 ){ //バッファ全域を使うように書き換える
		int n = 0;
		if ( mIndexBuffer ){ //インデクスバッファがあればインデクスで
			n = mIndexBuffer.indexNumber();
		}else{
			n = mVertexBuffer.vertexNumber();
		}
		switch ( mPrimitiveType ){
			case PRIMITIVE_TRIANGLE:
				ASSERT( n % 3 == 0 );
				n /= 3;
				break;
			case PRIMITIVE_LINE:
				ASSERT( n % 2 == 0 );
				n /= 2;
				break;
		}
		mPrimitiveNumber = n;
	}
}

Batch::Batch() : mImpl( 0 ){
}

Batch::Batch( const Batch& o ) : mImpl( o.mImpl ){
	if ( mImpl ){
		mImpl->container()->refer();
	}
}

Batch::~Batch(){
	release();
}

void Batch::release(){
	if ( mImpl ){
		Container::Impl* c = mImpl->container();
		c->release();
		if ( c->referenceCount() == 0 ){
			SAFE_DELETE( c );
		}
	}
	mImpl = 0;
}

Batch& Batch::operator=( const Batch& o ){
	release();
	mImpl = o.mImpl;
	if ( mImpl ){
		mImpl->container()->refer();
	}
	return *this;
}

bool Batch::operator==( const Batch& o ) const {
	return ( mImpl == o.mImpl );
}

bool Batch::operator!=( const Batch& o ) const {
	return ( mImpl != o.mImpl );
}

Batch::operator void*() const {
	return ( mImpl == 0 ) ? 0 : reinterpret_cast< void* >( ~0 );
}

Batch::Batch( Impl* impl ) : mImpl( impl ){
	if ( mImpl ){
		mImpl->container()->refer();
	}
}

void Batch::draw( 
const Vector3& color,
float transparency ) const {
	mImpl->draw( color, transparency );
}

void Batch::draw() const {
	mImpl->draw();
}

} //namespace Scene
} //namespace GameLib