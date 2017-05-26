#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
using namespace GameLib;

Framework::BlendMode gBlendMode = Framework::BLEND_LINEAR;
int gCount = 0;

namespace GameLib{
	void Framework::update(){
		sleep( 16 );
		//1枚目は不透明
		setBlendMode( BLEND_OPAQUE );
		double p00[ 2 ] = { 100.0, 100.0 };
		double p01[ 2 ] = { 200.0, 120.0 };
		double p02[ 2 ] = { 120.0, 200.0 };
		drawTriangle2D( p00, p01, p02, 0, 0, 0, 0xffff8080, 0xff80ff80, 0xff8080ff );

		//2枚目はお好みで
		setBlendMode( gBlendMode );  
		double p10[ 2 ] = { 110.0, 110.0 };
		double p11[ 2 ] = { 210.0, 130.0 };
		double p12[ 2 ] = { 130.0, 210.0 };
		unsigned alpha = ( gCount % 256 ) << 24;
		++gCount;  
		drawTriangle2D( 
			p10, p11, p12, 
			0, 0, 0, 
			0x80ff80 | alpha, 
			0x8080ff | alpha, 
			0xff8080 | alpha );

		//スペースで切り替え
		if ( Input::Manager::instance().keyboard().isTriggered( ' ' ) ){
			switch ( gBlendMode ){
				case BLEND_LINEAR: gBlendMode = BLEND_ADDITIVE; break;
				case BLEND_ADDITIVE: gBlendMode = BLEND_OPAQUE; break;
				case BLEND_OPAQUE: gBlendMode = BLEND_LINEAR; break;
			}
		}
		drawDebugString( 0, 0, "press SPACE to change blend mode." );
	}
}
