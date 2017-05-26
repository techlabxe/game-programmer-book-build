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
		srcX = 96; 
		srcY = 32;
	}else if ( mFlags & FLAG_BRICK ){
		if ( mFlags & ( FLAG_FIRE_X | FLAG_FIRE_Y ) ){ //Ä‚¯‚Ä‚é
			srcX = 0; 
			srcY = 96;
		}else{
			srcX = 0; 
			srcY = 64;
		}
	}else{
		srcX = 32; 
		srcY = 64; 
		floor = true;
	}
	image->draw( x*32, y*32, srcX, srcY, 32, 32 );
	//ƒIƒvƒVƒ‡ƒ“•`‰æ
	if ( floor ){
		srcX = -1; //”Ô•º
		if ( ( mFlags & FLAG_BOMB ) && !( mFlags & FLAG_EXPLODING ) ){
			srcX = 64;
			srcY = 64;
		}else if ( mFlags & FLAG_ITEM_BOMB ){
			srcX = 64;
			srcY = 0;
		}else if ( mFlags & FLAG_ITEM_POWER ){
			srcX = 96;
			srcY = 0;
		}
		if ( srcX != -1 ){
			image->draw( x*32, y*32, srcX, srcY, 32, 32 );
		}
	}
}

void StaticObject::drawExplosion( int x, int y, const Image* image ) const {
	int srcX = -1;
	int srcY = -1;
	if ( !( mFlags & FLAG_WALL ) && !( mFlags & FLAG_BRICK ) ){ //•Ç‚Ìã‚É‚Í”š•—‚Í•`‚©‚È‚¢
		if ( mFlags & FLAG_EXPLODING ){
			srcX = 96;
			srcY = 64;
		}else if ( mFlags & FLAG_FIRE_X ){
			if ( mFlags & FLAG_FIRE_Y ){
				srcX = 96;
				srcY = 64;
			}else{
				srcX = 0;
				srcY = 32;
			}
		}else if ( mFlags & FLAG_FIRE_Y ){
			srcX = 32;
			srcY = 32;
		}
	}
	if ( srcX != -1 ){
		image->draw( x*32, y*32, srcX, srcY, 32, 32 );
	}
}
