#include "GameLib/GameLib.h"
using namespace GameLib;

#include "Game/StaticObject.h"
#include "Image.h"

StaticObject::StaticObject() :
mCount( 0 ),
mBombOwner( 0 ),
mFlags( 0 ){
}

void StaticObject::setFlag( unsigned f ){
	mFlags |= f;
}

void StaticObject::resetFlag( unsigned f ){
	mFlags &= ~f;
}

bool StaticObject::checkFlag( unsigned f ) const {
	return ( mFlags & f ) ? true : false;
}

void StaticObject::draw( int x, int y, const Image* image ) const {
	int srcX = -1;
	int srcY = -1;
	bool floor = false;
	if ( mFlags & FLAG_WALL ){
		srcX = 48; 
		srcY = 16;
	}else if ( mFlags & FLAG_BRICK ){
		if ( mFlags & ( FLAG_FIRE_X | FLAG_FIRE_Y ) ){ //Ä‚¯‚Ä‚é
			srcX = 0; 
			srcY = 48;
		}else{
			srcX = 0; 
			srcY = 32;
		}
	}else{
		srcX = 16; 
		srcY = 32; 
		floor = true;
	}
	image->draw( x*16, y*16, srcX, srcY, 16, 16 );
	//ƒIƒvƒVƒ‡ƒ“•`‰æ
	if ( floor ){
		srcX = -1; //”Ô•º
		if ( ( mFlags & FLAG_BOMB ) && !( mFlags & FLAG_EXPLODING ) ){
			srcX = 32;
			srcY = 32;
		}else if ( mFlags & FLAG_ITEM_BOMB ){
			srcX = 32;
			srcY = 0;
		}else if ( mFlags & FLAG_ITEM_POWER ){
			srcX = 48;
			srcY = 0;
		}
		if ( srcX != -1 ){
			image->draw( x*16, y*16, srcX, srcY, 16, 16 );
		}
	}
}

void StaticObject::drawExplosion( int x, int y, const Image* image ) const {
	int srcX = -1;
	int srcY = -1;
	if ( !( mFlags & FLAG_WALL ) && !( mFlags & FLAG_BRICK ) ){ //•Ç‚Ìã‚É‚Í”š•—‚Í•`‚©‚È‚¢
		if ( mFlags & FLAG_EXPLODING ){
			srcX = 48;
			srcY = 32;
		}else if ( mFlags & FLAG_FIRE_X ){
			if ( mFlags & FLAG_FIRE_Y ){
				srcX = 48;
				srcY = 32;
			}else{
				srcX = 0;
				srcY = 16;
			}
		}else if ( mFlags & FLAG_FIRE_Y ){
			srcX = 16;
			srcY = 16;
		}
	}
	if ( srcX != -1 ){
		image->draw( x*16, y*16, srcX, srcY, 16, 16 );
	}
}
