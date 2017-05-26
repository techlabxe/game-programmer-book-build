#include "GameLib/GameLib.h"
#include "GameLib/Scene/Font.h"
#include "Scene/FontImpl.h"
#include "Scene/FontImplOS.h"
#include "Scene/FontImplBitmap.h"

namespace GameLib{
namespace Scene{

Font Font::create(
const char** textureFileNames,
int textureNumber,
BitmapChar* bitmapChars,
int bitmapCharNumber ){
	Font r;
	r.mImpl = NEW FontImplBitmap( textureFileNames, textureNumber, bitmapChars, bitmapCharNumber );
	return r;
}

Font Font::create(
const char** textureFileData,
const int* textureFileSizes,
int textureNumber,
BitmapChar* bitmapChars,
int bitmapCharNumber ){
	Font r;
	r.mImpl = NEW FontImplBitmap( textureFileData, textureFileSizes, textureNumber, bitmapChars, bitmapCharNumber );
	return r;
}

Font Font::create(
const char* osFontName,
int charHeightHint,
bool bold,
bool italic,
bool proportional,
bool antialias,
int textureWidth,
int textureHeight ){
	Font r;
	r.mImpl = NEW FontImplOS( 
		osFontName, 
		charHeightHint, 
		bold, 
		italic, 
		proportional,
		antialias,
		textureWidth,
		textureHeight );
	return r;
}

bool Font::getChar(
Graphics::Texture* t,
Vector2* uv0,
Vector2* uv1,
int c ){
	ASSERT( mImpl && "Empty Object" );
	return mImpl->getChar( t, uv0, uv1, c );
}

int Font::charHeight() const {
	ASSERT( mImpl && "Empty Object" );
	return mImpl->charHeight();
}

#define TYPE Font
#include "GameLib/Base/Impl/ReferenceTypeTemplate.h"

} //namespace Scene
} //namespace GameLib
