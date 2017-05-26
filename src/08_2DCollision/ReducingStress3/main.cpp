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
Square gPlayer; //自キャラのつもり
Square gWall[ 5 ][ 5 ];

namespace GameLib{
	void Framework::update(){
		if ( gFirstFrame ){
			setFrameRate( 60 );
			gFirstFrame = false;
			for ( int i = 0; i < 5; ++i ){
				for ( int j = 0; j < 4; ++j ){
					gWall[ i ][ j ].set( 
						48 + i * 64,
						16 + j * 64,
						16 );
				}
				gWall[ i ][ 4 ].set( 48 + i * 64, 16 + 2 * 64 + 32, 16 );
			}
			gPlayer.set( 16, 16, 15 );
		}
		int dx = 0;
		int dy = 0;
		if ( isKeyOn( 'a' ) ){
			dx -= 1;
		}else if ( isKeyOn( 's' ) ){
			dx += 1;
		}
		if ( isKeyOn( 'w' ) ){
			dy -= 1;
		}else if ( isKeyOn( 'z' ) ){
			dy += 1;
		}
		//衝突処理
		unsigned color = 0xffff0000;
		Square movedPlayerX = gPlayer; //X移動
		movedPlayerX.mX += dx;
		Square movedPlayerY = gPlayer; //Y移動
		movedPlayerY.mY += dy;
		bool hitX = false;
		bool hitY = false;
		for ( int i = 0; i < 5; ++i ){
			for ( int j = 0; j < 5; ++j ){
				if ( movedPlayerX.isIntersect( gWall[ i ][ j ] ) ){
					hitX = true;
					color = 0xffffffff;
				}
				if ( movedPlayerY.isIntersect( gWall[ i ][ j ] ) ){
					hitY = true;
					color = 0xffffffff;
				}
			}
		}
		if ( hitX && !hitY ){
			gPlayer = movedPlayerY; //Yはオーケー
		}else if ( !hitX && hitY ){ 
			gPlayer = movedPlayerX; //Xはオーケー
		}else{ //ダメです。普通にやる。
			gPlayer.mX += dx;
			gPlayer.mY += dy;
			bool hit = false;
			for ( int i = 0; i < 5; ++i ){
				for ( int j = 0; j < 5; ++j ){
					if ( gPlayer.isIntersect( gWall[ i ][ j ] ) ){
						hit = true;
						color = 0xffffffff;
					}
				}
			}
			if ( hit ){
				gPlayer.mX -= dx;
				gPlayer.mY -= dy;
			}
		}
		//描画
		unsigned* vram = videoMemory();
		//一旦クリア
		for ( int i = 0; i < width() * height(); ++i ){
			vram[ i ] = 0;
		}
		//動かないほう描画
		for ( int i = 0; i < 5; ++i ){
			for ( int j = 0; j < 5; ++j ){
				for ( int y = 0; y < 32; ++y ){
					for ( int x = 0; x < 32; ++x ){
						int tx = x + gWall[ i ][ j ].mX - 16;
						int ty = y + gWall[ i ][ j ].mY - 16;
						vram[ ty * width() + tx ] = 0xff00ff00;
					}
				}
			}
		}
		//動くほう描画
		for ( int y = 0; y < 32; ++y ){
			for ( int x = 0; x < 32; ++x ){
				int tx = x + gPlayer.mX - 16;
				int ty = y + gPlayer.mY - 16;
				vram[ ty * width() + tx ] = color; 
			}
		}
	}
}