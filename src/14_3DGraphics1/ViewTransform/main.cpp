#include <sstream>
#include "GameLib/Framework.h"
#include "Vector3.h"
#include "Matrix34.h"
#include "GameLib/GameLibMath.h"
using namespace GameLib;
using namespace std;

Vector3 gEyePosition( 0.0, 0.0, 1.0 );
Vector3 gEyeTarget( 0.0, 0.0, 0.0 );
int gCount;

namespace GameLib{
	void Framework::update(){
		setFrameRate( 60 );
		//元の頂点配列
		Vector3 p[ 4 ];
		unsigned c[ 4 ]; //わかりやすいように色
		p[ 0 ].set( -1000.0, 0.0, -1000.0 );
		p[ 1 ].set( -1000.0, 0.0, 1000.0 );
		p[ 2 ].set( 1000.0, 0.0, -1000.0 );
		p[ 3 ].set( 1000.0, 0.0, 1000.0 );
		c[ 0 ] = 0xffff0000; //赤
		c[ 1 ] = 0xff00ff00; //緑
		c[ 2 ] = 0xff0000ff; //青
		c[ 3 ] = 0xffffffff; //白

		//ビュー行列を作ろう
		Matrix34 m;
		m.setViewTransform( gEyePosition, gEyeTarget ); //この中が本体なのでそっちを参照のこと

		//行列にベクタをかけて回る
		for ( int i = 0; i < 4; ++i ){
			m.multiply( &p[ i ], p[ i ] );
		}

		//ニアクリップとファークリップ
		const double nearClip = 1.0;
		const double farClip = 10000.0;
		//ニアとファーからZ範囲変換の式を作る
		const double zConvA = 1.0 / ( nearClip - farClip ); //1/(n-f)
		const double zConvB = nearClip * zConvA; //n/(n-f)

		//ハードウェアに渡す準備をする。4次元化
		double p4[ 4 ][ 4 ];
		for ( int i = 0; i < 4; ++i ){
			p4[ i ][ 0 ] = p[ i ].x;
			//yに640/480をかけて縦横比を調整
			p4[ i ][ 1 ] = p[ i ].y * 640.0 / 480.0;
			//wに範囲変換前のzを「マイナスにして」格納。Z軸が手前向きだとこのマイナスが必要。
			p4[ i ][ 3 ] = -p[ i ].z;
			//z範囲変換
			p4[ i ][ 2 ] = zConvA * p[ i ].z + zConvB; //範囲変換もZ軸の向きを
			//zにwをかけておく。
			p4[ i ][ 2 ] *= p4[ i ][ 3 ];
		}
		//四角形を描く。
		drawTriangle3DH( p4[ 0 ], p4[ 1 ], p4[ 2 ], 0, 0, 0, c[ 0 ], c[ 1 ], c[ 2 ] );
		drawTriangle3DH( p4[ 3 ], p4[ 1 ], p4[ 2 ], 0, 0, 0, c[ 3 ], c[ 1 ], c[ 2 ] );

		++gCount;

		//視点と注視点を適当にいじる
		gEyePosition.x = sin( gCount ) * 2000;
		gEyePosition.z = cos( gCount ) * 2000;
		gEyePosition.y = 1000.f;
		gEyeTarget.x = gCount % 100;
		gEyeTarget.y = gCount % 200;
		gEyeTarget.z = gCount % 300;
	}
}
