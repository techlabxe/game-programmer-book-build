#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Mouse.h"
#include <sstream>
using namespace std;

namespace GameLib{
	void Framework::update(){
		using namespace Input;
		Mouse mouse;
		mouse = Input::Manager::instance().mouse();
		ostringstream oss;
		int w = mouse.wheel();
		int x = mouse.x();
		int y = mouse.y();
		bool b0 = mouse.isOn( Mouse::BUTTON_LEFT );
		bool b1 = mouse.isOn( Mouse::BUTTON_RIGHT );
		bool b2 = mouse.isOn( Mouse::BUTTON_MIDDLE );

		oss.str("");
		oss << x << " " << y << " " << b0 << ":" << b1 << ":" << b2 << ":" << w;
		drawDebugString( 0, 1, oss.str().c_str() );

		//だんだん消してく
		unsigned* vram = videoMemory();
		for ( int i = 0; i < height(); ++i ){
			for ( int j = 0; j < width(); ++j ){
				unsigned c = vram[ i * width() + j ];
				if ( c > 0 ){
					c -= 0x01010101;
					vram[ i * width() + j ] = c;
				}
			}
		}
		//マウスカーソルを出してみるよ
		if ( x >= 0 && x < width() && y >= 0 && y < height() ){
			vram[ y * width() + x ] = 0xffffffff;
		}
	}
}