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
Square gWall; //壁

namespace GameLib{
	void Framework::update(){
		if ( gFirstFrame ){
			setFrameRate( 60 );
			gFirstFrame = false;
			gPlayer.set( 16, 16, 16 );
			gWall.set( 160, 120, 16 );
		}
		//移動量ゲット
		int dx = 0;
		int dy = 0;
		if ( isKeyOn( 'a' ) ){
			dx = -7;
		}else if ( isKeyOn( 's' ) ){
			dx = 7;
		}
		if ( isKeyOn( 'w' ) ){
			dy = -7;
		}else if ( isKeyOn( 'z' ) ){
			dy = 7;
		}
		unsigned* vram = videoMemory();
		//衝突処理(最大4回)
		unsigned color = 0xffff0000;
		int numerator = 1; //分子
		int denominator = 1; //分母
		int testDx = dx; //これから試すdx,dy
		int testDy = dy;
		int lastDx = 0; //大丈夫だった最大のdx,dy
		int lastDy = 0;
		for ( int i = 0; i < 4; ++i ){
			Square tSquare; //テンポラリ
			tSquare.set( gPlayer.mX + testDx, gPlayer.mY + testDy, gPlayer.mHalfSize );
			numerator *= 2;
			denominator *= 2;
			if ( tSquare.isIntersect( gWall ) ){
				color = 0xffffffff;
				numerator -= 1;
			}else{
				numerator += 1;
				lastDx = testDx; //更新
				lastDy = testDy;
			}
			testDx = dx * numerator / denominator;
			testDy = dy * numerator / denominator;
		}
		//移動
		gPlayer.mX += lastDx;
		gPlayer.mY += lastDy;

		//描画
		//一旦クリア
		for ( int i = 0; i < width() * height(); ++i ){
			vram[ i ] = 0;
		}
		//動かないほう描画
		for ( int y = 0; y < 32; ++y ){
			for ( int x = 0; x < 32; ++x ){
				int tx = x + gWall.mX - 16;
				int ty = y + gWall.mY - 16;
				vram[ ty * width() + tx ] = 0xff0000ff; 
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