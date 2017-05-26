#ifndef INCLUDED_IMAGE_H
#define INCLUDED_IMAGE_H

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
private:
	int mWidth;
	int mHeight;
	unsigned* mData;
};

#endif
