#include <d3d9.h>
#undef max
#undef min

#include "GameLib/GameLib.h"
#include "GameLib/Graphics/VertexBuffer.h"
#include "Graphics/VertexBufferImpl.h"
#include "Graphics/ManagerImpl.h"

namespace GameLib{
namespace Graphics{

VertexBuffer VertexBuffer::create( PseudoXml::ConstElement e ){
	VertexBuffer r;
	r.mImpl = NEW Impl( e, gManagerImpl->dxDevice() );
	return r;
}


VertexBuffer VertexBuffer::create( int vertexNumber ){
	VertexBuffer r;
	r.mImpl = NEW Impl( vertexNumber, gManagerImpl->dxDevice() );
	return r;
}

Vertex* VertexBuffer::lock(){
	ASSERT( mImpl && "Graphics::VertexBuffer : This is empty object." );
	return mImpl->lock();
}

void VertexBuffer::unlock( Vertex** p ){
	ASSERT( *p && "Graphics::VertexBuffer : give LOCKED pointer." );
	ASSERT( mImpl && "Graphics::VertexBuffer : This is empty object." );
	mImpl->unlock();
	*p = 0;
}

//NULLƒ|ƒCƒ“ƒ^‚©‚ç‚Ì•ÏŠ·
VertexBuffer::VertexBuffer( const Impl* ) : mImpl( 0 ){
}

const char* VertexBuffer::name() const {
	return mImpl->mName;
}

int VertexBuffer::vertexNumber() const {
	return mImpl->mVertexNumber;
}


#define TYPE VertexBuffer
#include "GameLib/Base/Impl/ReferenceTypeTemplate.h"

} //namespace Graphics
} //namespace GameLib
