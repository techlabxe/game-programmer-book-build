#include <d3d9.h>
#undef min
#undef max

#include "GameLib/GameLib.h"
#include "GameLib/Graphics/Manager.h"
#include "GameLib/WindowCreator/WindowCreator.h"

#include "Graphics/ManagerImpl.h"

namespace GameLib{
using namespace Math;
namespace Graphics{

ManagerImpl* gManagerImpl = 0;

Manager::Manager(){
	//別スレッドからの呼び出しは許さない
	ASSERT( WindowCreator::isMainThread() && "you must call from MAIN thread" );
}

void Manager::create( 
void* hwnd, 
int w, 
int h,
bool fullScreen,
bool vSync,
bool antiAlias ){
	STRONG_ASSERT( !gManagerImpl );
	gManagerImpl = NEW ManagerImpl( 
		static_cast< HWND >( hwnd ), 
		w, 
		h,
		fullScreen,
		vSync,
		antiAlias );
}

void Manager::destroy(){
	SAFE_DELETE( gManagerImpl );
}

void Manager::beginDraw(){
	gManagerImpl->beginDraw();
}

void Manager::endDraw(){
	gManagerImpl->endDraw();
}

Manager Manager::instance(){
	return Manager();
}

void Manager::setTexture( Texture o ){
	gManagerImpl->setTexture( o.mImpl );
}

void Manager::setVertexBuffer( VertexBuffer o ){
	gManagerImpl->setVertexBuffer( o.mImpl );
}

void Manager::setIndexBuffer( IndexBuffer o ){
	gManagerImpl->setIndexBuffer( o.mImpl );
}

void Manager::enableDepthTest( bool f ){
	gManagerImpl->enableDepthTest( f );
}

void Manager::enableDepthWrite( bool f ){
	gManagerImpl->enableDepthWrite( f );
}

void Manager::setBlendMode( BlendMode b ){
	gManagerImpl->setBlendMode( b );
}

void Manager::setCullMode( CullMode c ){
	gManagerImpl->setCullMode( c );
}

void Manager::setTextureFilter( TextureFilter tf ){
	gManagerImpl->setTextureFilter( tf );
}

void Manager::setProjectionViewMatrix( const Matrix44& m ){
	gManagerImpl->setProjectionViewMatrix( m );
}

void Manager::setWorldMatrix( const Matrix34& m ){
	gManagerImpl->setWorldMatrix( m );
}

void Manager::setViewport( int x, int y, int w, int h ){
	gManagerImpl->setViewport( x, y, w, h );
}

void Manager::getViewport( int* x, int* y, int* w, int* h ){
	gManagerImpl->getViewport( x, y, w, h );
} 

void Manager::blendToScreen( Texture t ){
	gManagerImpl->blendToScreen( t.mImpl );
}

void Manager::draw( int offset, int primitiveNumber, PrimitiveType p ){
	gManagerImpl->draw( offset, primitiveNumber, p );
}

void Manager::drawIndexed( int offset, int primitiveNumber, PrimitiveType p ){
	gManagerImpl->drawIndexed( offset, primitiveNumber, p );
}

int Manager::width() const { 
	return gManagerImpl->mWidth;
}

int Manager::height() const {
	return gManagerImpl->mHeight;
}
 
int Manager::fullScreenWindowWidth() const {
	return gManagerImpl->mFullScreenWindowWidth;
}

int Manager::fullScreenWindowHeight() const {
	return gManagerImpl->mFullScreenWindowHeight;
}

bool Manager::canRender() const {
	return gManagerImpl->mCanRender;
}

void Manager::restore(){
	gManagerImpl->restore();
}

void Manager::enableFullScreen( bool f ){
	gManagerImpl->enableFullScreen( f );
}

void Manager::setLightingMode( LightingMode lm ){
	gManagerImpl->setLightingMode( lm );
}

unsigned Manager::frameId() const {
	return gManagerImpl->mFrameId;
}

void Manager::captureScreen( const char* filename ){
	gManagerImpl->mCaptureRequest = true;
	gManagerImpl->mCaptureFilename = filename;
}

void Manager::getPointerModifier( float* scale, Vector2* offset ) const {
	gManagerImpl->getPointerModifier( scale, offset );
}

void Manager::setLightPosition( int index, const Vector3& position ){
	ASSERT( index >= 0 && index < 4 );
	gManagerImpl->setLightPosition( index, position );
}

void Manager::setLightIntensity( int index, float intensity ){
	ASSERT( index >= 0 && index < 4 );
	gManagerImpl->setLightIntensity( index, intensity );
}

void Manager::setLightColor( int index, const Vector3& color ){
	ASSERT( index >= 0 && index < 4 );
	gManagerImpl->setLightColor( index, color );
}

void Manager::setEyePosition( const Vector3& position ){
	gManagerImpl->setEyePosition( position );
}

void Manager::setDiffuseColor( const Vector3& c ){
	gManagerImpl->setDiffuseColor( c );
}

void Manager::setSpecularColor( const Vector3& c ){
	gManagerImpl->setSpecularColor( c );
}

void Manager::setEmissionColor( const Vector3& c ){
	gManagerImpl->setEmissionColor( c );
}

void Manager::setAmbientColor( const Vector3& c ){
	gManagerImpl->setAmbientColor( c );
}

void Manager::setTransparency( float t ){
	gManagerImpl->setTransparency( t );
}

void Manager::setSpecularSharpness( float e ){
	gManagerImpl->setSpecularSharpness( e );
}


} //namespace Graphics
} //namespace GameLib