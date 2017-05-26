#include "GameLib/Framework.h"
using namespace GameLib;
#include "Stage.h"
#include "Matrix44.h"
#include "Vector3.h"

Stage::Stage() : mTexture( 0 ){
	Framework::instance().createTexture( &mTexture, "stage.tga" );
}

Stage::~Stage(){
	Framework::instance().destroyTexture( &mTexture );
}

void Stage::draw( const Matrix44& pvm ) const {
	Vector3 p[ 4 ];
	p[ 0 ].set( -50.0, 0.0, -50.0 );
	p[ 1 ].set( -50.0, 0.0, 50.0 );
	p[ 2 ].set( 50.0, 0.0, -50.0 );
	p[ 3 ].set( 50.0, 0.0, 50.0 );
	double uv[ 4 ][ 2 ] = {
		{ 0.0, 0.0, },
		{ 0.0, 1.0, },
		{ 1.0, 0.0, },
		{ 1.0, 1.0  },
	};

	//行列にベクタをかけて回る
	double p4[ 4 ][ 4 ];
	for ( int i = 0; i < 4; ++i ){
		pvm.multiply( p4[ i ], p[ i ] );
	}
	GameLib::Framework f = GameLib::Framework::instance();
	f.enableDepthTest( true );
	f.setTexture( mTexture );
	f.drawTriangle3DH( p4[ 0 ], p4[ 1 ], p4[ 2 ], uv[ 0 ], uv[ 1 ], uv[ 2 ] );
	f.drawTriangle3DH( p4[ 3 ], p4[ 1 ], p4[ 2 ], uv[ 3 ], uv[ 1 ], uv[ 2 ] );
}
