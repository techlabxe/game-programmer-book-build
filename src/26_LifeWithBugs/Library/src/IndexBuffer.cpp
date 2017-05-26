#include "GameLib/GameLib.h"
#include "Library/IndexBuffer.h"
#include "GameLib/Graphics/Manager.h"
#include "GameLib/Graphics/IndexBuffer.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
using namespace GameLib::PseudoXml;

IndexBuffer::IndexBuffer( Element& e ){
	//エラーチェック全然してません
	int n = e.childNumber();
	mSize = n * 3; //三角形数×3
	if ( mSize > 0 ){
		mIndexBuffer = GameLib::Graphics::IndexBuffer::create( mSize );
		unsigned short* indices = mIndexBuffer.lock();
		for ( int i = 0; i < n; ++i ){
			Element triangle = e.child( i );
			Attribute a = triangle.attribute( 0 ); //indices一個しかないので決めうち
			//unsigned shortを取る関数がないのでintに入れてからコピー
			int tmp[ 3 ];
			a.getIntValues( tmp, 3 ); //エラーチェックしないといけないんですよ？本当は。
			indices[ i * 3 + 0 ] = static_cast< unsigned short >( tmp[ 0 ] );
			indices[ i * 3 + 1 ] = static_cast< unsigned short >( tmp[ 1 ] );
			indices[ i * 3 + 2 ] = static_cast< unsigned short >( tmp[ 2 ] );
		}
		mIndexBuffer.unlock( &indices );
	}
	//名前を抜く
	int an = e.attributeNumber();
	for ( int i = 0; i < an; ++i ){
		Attribute a = e.attribute( i );
		string name = a.name();
		if ( name == "name" ){ 
			mName = a.value();
		}
	}
}

IndexBuffer::~IndexBuffer(){
	//親が消えるとインデクスバッファも勝手に消える。
}

int IndexBuffer::size() const {
	return mSize;
}

const string* IndexBuffer::name() const {
	return &mName;
}

void IndexBuffer::set() const {
	GameLib::Graphics::Manager::instance().setIndexBuffer( mIndexBuffer );
}
