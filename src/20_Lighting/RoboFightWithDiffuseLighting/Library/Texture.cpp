#include "Library/Texture.h"
#include "GameLib/Framework.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
using namespace GameLib::PseudoXml;

Texture::Texture( Element& e ) : mTexture( 0 ){
	//名前やらファイル名やらを抜く
	int an = e.attributeNumber();
	for ( int i = 0; i < an; ++i ){
		Attribute a = e.attribute( i );
		string name = a.name();
		if ( name == "name" ){
			mName = a.value();
		}else if ( name == "filename" ){
			GameLib::Framework::instance().createTexture( 
				&mTexture,
				a.value() );
		}
	}
}


Texture::Texture( const char* filename ) : mTexture( 0 ){
	GameLib::Framework::instance().createTexture( 
		&mTexture,
		filename );
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
