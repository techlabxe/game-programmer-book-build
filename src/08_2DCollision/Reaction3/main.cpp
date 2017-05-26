#include "GameLib/Framework.h"

class Square{
public:
	void set( int x, int y, int halfSize ){
		mX = x;
		mY = y;
		mHalfSize = halfSize;
	}
	//1000分の1フレーム単位の当たった時刻を返す。0から1000までの整数を返し、
	//0なら移動開始の瞬間に当たったことを、1000は当たらなかったことを意味する。
	int getIntersectionTime( int dx, int dy, const Square& b ) const {
		//自分が原点にいるかのように二つを移動する。
		int x = b.mX - mX;
		int y = b.mY - mY;
		//自分の速度が0になるように、相手に速度を押し付ける。
		int vx = -dx;
		int vy = -dy;
		//大きさを自分に集める。正方形なので単純に足せば良い。
		int s = mHalfSize + b.mHalfSize;
		//この状況で(x,y)+t(vx,vy)が4本の直線
		//x = -s, x = s, y = -s, y = s
		//に当たる時刻を計算する。

		int tx0, tx1, ty0, ty1; //4つの時刻
		//x+t*vx=-sとx+t*vx=sより、
		if ( vx == 0 ){
			//今交差状態かどうかを調べる
			if ( -s < x && x < s ){
				tx0 = -1000; //過去にあたり、未来に抜ける、の意味
				tx1 = 1000;
			}else{ //そうでなければ未来永劫当たらない
				tx0 = tx1 = 1000;
			}
		}else{
			tx0 = ( -s - x ) * 1000 / vx;
			tx1 = ( s - x ) * 1000 / vx;
		}
		//y+t*vy=-sとy+t*vy=sより、
		if ( vy == 0 ){
			//今交差状態かどうかを調べる
			if ( -s < y && y < s ){
				ty0 = -1000; //過去にあたり、未来に抜ける、の意味
				ty1 = 1000;
			}else{ //そうでなければ未来永劫当たらない
				ty0 = ty1 = 1000;
			}
		}else{
			ty0 = ( -s - y ) * 1000 / vy;
			ty1 = ( s - y ) * 1000 / vy;
		}
		//tx0,ty0を小さいほうにする。後が楽になる。
		int t;
		if ( tx0 > tx1 ){
			t = tx0;
			tx0 = tx1;
			tx1 = t;
		}
		if ( ty0 > ty1 ){
			t = ty0;
			ty0 = ty1;
			ty1 = t;
		}
		t = 1000; //衝突時刻
		//tx0にて当たるかチェック
		if ( tx0 >= 0 && tx0 < 1000 ){
			//予定時刻においてy軸が交差状態にあればあたりである。
			//これはty0<=tx0<=ty1を満たせばいい。
			if ( ty0 <= tx0 && tx0 <= ty1 && tx0 < t ){
				t = tx0;
			}
		}
		//tx1
		if ( tx1 >= 0 && tx1 < 1000 ){
			if ( ty0 <= tx0 && tx0 <= ty1 && tx1 < t ){
				t = tx1;
			}
		}
		//ty0
		if ( ty0 >= 0 && ty0 < 1000 ){
			if ( tx0 <= ty0 && ty0 <= tx1 && ty0 < t ){
				t = ty0;
			}
		}
		//ty1
		if ( ty1 >= 0 && ty1 < 1000 ){
			if ( tx0 <= ty0 && ty0 <= tx1 && ty1 < t ){
				t = ty1;
			}
		}
		return t;
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
		//衝突処理
		unsigned color = 0xffff0000;
		int time = gPlayer.getIntersectionTime( dx, dy, gWall );
		if ( time < 1000 ){
			color = 0xffffffff;
		}
		//移動
		gPlayer.mX += ( dx * time ) / 1000;
		gPlayer.mY += ( dy * time ) / 1000;
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