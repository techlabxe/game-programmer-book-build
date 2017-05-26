#include "GameLib/GameLib.h"
#include "Sequence/Game/Child.h"
#include "Sequence/Game/Parent.h"

namespace Sequence{
namespace Game{

Child::~Child(){
}

Base* Child::update( Base* p ){
	Parent* parent = dynamic_cast< Parent* >( p );
	ASSERT( parent );
	return update( parent );
}

} //namespace Game
} //namespace Sequence
