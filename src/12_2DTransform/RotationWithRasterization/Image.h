#ifndef INCLUDED_IMAGE_H
#define INCLUDED_IMAGE_H

class Vector2;

class Image{
public:
	Image( const char* filename );
	~Image();
	int width() const;
	int height() const;
	unsigned pixel( const Vector2& ) const;
private:
	int mWidth;
	int mHeight;
	unsigned* mData;
};

#endif
