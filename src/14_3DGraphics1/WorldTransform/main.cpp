#include "GameLib/Framework.h"
#include "Vector3.h"
#include "Matrix34.h"

#include <sstream>
using namespace std;

Vector3 gTranslation( 0.0, 0.0, 0.0 );
Vector3 gScale( 1.0, 1.0, 1.0 );
int gCount;

namespace GameLib{
	void Framework::update(){
		setFrameRate( 60 );

		//元の頂点配列
		Vector3 p[ 4 ];
		p[ 0 ].set( -500.0, -300.0, -1000.0 );
		p[ 1 ].set( -500.0, -300.0, 1000.0 );
		p[ 2 ].set( 500.0, -300.0, -1000.0 );
		p[ 3 ].set( 500.0, -300.0, 1000.0 );
		//行列を作ろう
		//拡大してから移動する。逆順だから、移動を作って、拡大をかける。
		Matrix34 m;
		m.setTranslation( gTranslation );
		m.scale( gScale );
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
		drawTriangle3DH( p4[ 0 ], p4[ 1 ], p4[ 2 ] );
		drawTriangle3DH( p4[ 3 ], p4[ 1 ], p4[ 2 ] );

		//移動量、拡大率適当にいじる
		gTranslation.x = gCount % 1000 - 500;
		gTranslation.y = gCount % 800 - 400;
		gTranslation.z = gCount % 600 - 300 - 2000;
		gScale.x = ( gCount % 500 ) / 500.0 + 0.5;
		gScale.y = ( gCount % 300 ) / 300.0 + 0.5;
		gScale.z = ( gCount % 100 ) / 100.0 + 0.5;

		++gCount;
	}
}
