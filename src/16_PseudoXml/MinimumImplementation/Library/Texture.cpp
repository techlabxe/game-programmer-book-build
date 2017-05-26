#include "GameLib/Framework.h"
#include "PseudoXml/Element.h"
#include "PseudoXml/Attribute.h"
#include "Library/Texture.h"
using namespace PseudoXml;

Texture::Texture( const Element* e ) :
mTexture( 0 ){
	int an = e->attributeNumber();
	for ( int i = 0; i < an; ++i ){
		const Attribute* a = e->attribute( i );
		const string* name = a->name();
		if ( *name == "filename" ){
			GameLib::Framework::instance().createTexture( 
				&mTexture,
				a->value()->c_str() );
		}else if ( *name == "name" ){
			mName = *a->value();
		}
	}
}

Texture::~Texture(){
	GameLib::Framework::instance().destroyTexture( &mTexture );
}

void Texture::set() const {
	GameLib::Framework::instance().setTexture( mTexture );
}

const string* Texture::name() const {
	return &mName;
}


