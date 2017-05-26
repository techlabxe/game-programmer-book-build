#ifndef INCLUDED_IMAGE_H
#define INCLUDED_IMAGE_H

namespace GameLib{
	class Texture;
}

class Image{
public:
	Image( const char* filename );
	~Image();
	int width() const;
	int height() const;
	void draw( 
		int dstX, 
		int dstY, 
		int srcX, 
		int srcY, 
		int width, 
		int height ) const;
	void draw() const;
private:
	int mWidth;
	int mHeight;
	int mTextureWidth;
	int mTextureHeight;
	GameLib::Texture* mTexture;
};

#endif
