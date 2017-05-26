#ifndef INCLUDED_SQUARE_H
#define INCLUDED_SQUARE_H

class Square{
public:
	Square();
	Square( int x, int y, int halfSize );
	void set( int x, int y, int halfSize );
	bool isIntersect( const Square& b ) const;
	//“–‚½‚ç‚¸‚É“®‚¯‚éŒÀŠE‚ğ’²‚×‚éB
	void getMoveLimit( int* dx, int* dy, const Square& wall, int iteration = 1 ) const;

	int mX;
	int mY;
	int mHalfSize;
};

#endif
