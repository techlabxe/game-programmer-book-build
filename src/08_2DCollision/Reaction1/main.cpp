#include "GameLib/Framework.h"

class Square{
public:
	void set( int x, int y, int halfSize ){
		mX = x;
		mY = y;
		mHalfSize = halfSize;
	}
	bool isIntersect( const Square& b ) const {
		int al = mX - mHalfSize; //left A
		int ar = mX + mHalfSize; //right A
		int bl = b.mX - b.mHalfSize; //left B
		int br = b.mX + b.mHalfSize; //right B
		if ( ( al < br ) && ( ar > bl ) ){
			int at = mY - mHalfSize; //top A
			int ab = mY + mHalfSize; //bottom A
			int bt = b.mY - b.mHalfSize; //top B
			int bb = b.mY + b.mHalfSize; //bottom B
			if ( ( at < bb ) && ( ab > bt ) ){
				return true;
			}
		}
		return false;
	}
	int mX;
	int mY;
	int mHalfSize;
};
bool gFirstFrame = true;
Square gPlayer; //©ƒLƒƒƒ‰‚Ì‚Â‚à‚è
Square gWall; //•Ç

namespace GameLib{
	void Framework::update(){
		if ( gFirstFrame ){
			setFrameRate( 60 );
			gFirstFrame = false;
			gPlayer.set( 16, 16, 16 );
			gWall.set( 160, 120, 16 );
		}
		//ˆÚ“®—ÊƒQƒbƒg
		int dx = 0;
		int dy = 0;
		if ( isKeyOn( 'a' ) ){
			dx = -1;
		}else if ( isKeyOn( 's' ) ){
			dx = 1;
		}
		if ( isKeyOn( 'w' ) ){
			dy = -1;
		}else if ( isKeyOn( 'z' ) ){
			dy = 1;
		}
		unsigned* vram = videoMemory();
		//ˆÚ“®
		gPlayer.mX += dx;
		gPlayer.mY += dy;
		//Õ“Ëˆ—
		unsigned color = 0xffff0000;
		if ( gPlayer.isIntersect( gWall ) ){
			color = 0xffffffff;
			//“®‚©‚È‚©‚Á‚½‚±‚Æ‚É‚·‚é
			gPlayer.mX -= dx;
			gPlayer.mY -= dy;
		}
		//•`‰æ
		//ˆê’UƒNƒŠƒA
		for ( int i = 0; i < width() * height(); ++i ){
			vram[ i ] = 0;
		}
		//“®‚©‚È‚¢‚Ù‚¤•`‰æ
		for ( int y = 0; y < 32; ++y ){
			for ( int x = 0; x < 32; ++x ){
				int tx = x + gWall.mX - 16;
				int ty = y + gWall.mY - 16;
				vram[ ty * width() + tx ] = 0xff0000ff; 
			}
		}
		//“®‚­‚Ù‚¤•`‰æ
		for ( int y = 0; y < 32; ++y ){
			for ( int x = 0; x < 32; ++x ){
				int tx = x + gPlayer.mX - 16;
				int ty = y + gPlayer.mY - 16;
				vram[ ty * width() + tx ] = color; 
			}
		}
	}
}