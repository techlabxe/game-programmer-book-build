#ifndef INCLUDED_GAMELIB_SCENE_TREETEMPLATE_H
#define INCLUDED_GAMELIB_SCENE_TREETEMPLATE_H

#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"

#include "Scene/BatchImpl.h"
#include "Scene/NodeTemplate.h"
#include "Scene/Node.h"

namespace GameLib{

namespace Math{
	class Vector3;
}
namespace PseudoXml{
	class Element;
}

namespace Scene{
using namespace Math;

//ツリーの原型。ライブラリの外からは見えないのでいつものImplはない。
class TreeTemplate{
public:
	TreeTemplate( PseudoXml::ConstElement&, Container::Impl& ); //これはcpp内。
	~TreeTemplate(){
		SAFE_DELETE_ARRAY( mNodes );
		mName = 0; //コンテナが持っている文字列なのでここでは参照をはずすだけ
		mContainer = 0;
	}
	int nodeNumber() const { 
		return mNodeNumber; 
	}
	const NodeTemplate* node( int i ) const { 
		return &mNodes[ i ]; 
	}
	const char* name() const { 
		return mName; 
	}
	Container::Impl* container(){
		return mContainer;
	}
private:
	int countNode( GameLib::PseudoXml::ConstElement&, int n );
	int build( PseudoXml::ConstElement&, Container::Impl&, int nodePos );

	NodeTemplate* mNodes; //0番が根と決まっているので根を指すポインタは不要
	int mNodeNumber;
	const char* mName;
	Container::Impl* mContainer;
};

} //namespace Scene
} //namespace GameLib

#endif
