#include "GameLib/GameLib.h"
#include "GameLib/Scene/Container.h"
#include "GameLib/Scene/Model.h"
#include "GameLib/Scene/Batch.h"
#include "GameLib/Scene/Tree.h"

#include "Scene/ContainerImpl.h"
#include "Scene/ModelImpl.h"
#include "Scene/TreeImpl.h"

namespace GameLib{
namespace Scene{

Container Container::create( 
PseudoXml::ConstElement e,
const char* extraDataPath ){
	Container r;
	r.mImpl = NEW Impl( e, extraDataPath );
	return r;
}

Container Container::create( const char* filename, const char* extraDataPath ){
	Container r;
	r.mImpl = NEW Impl( filename, extraDataPath );
	return r;
}

Model Container::createModel( const char* batchName ) const {
	Batch::Impl* batchImpl = mImpl->batch( batchName );
	Model::Impl* modelImpl = NEW Model::Impl( batchImpl );
	modelImpl->release(); //コンテナは管理しない
	return Model( modelImpl );
}

Model Container::createModel( int batchId ) const {
	Batch::Impl* batchImpl = mImpl->batch( batchId );
	Model::Impl* modelImpl = NEW Model::Impl( batchImpl );
	modelImpl->release(); //コンテナは管理しない
	return Model( modelImpl );
}

Tree Container::createTree( const char* treeName ) const {
	TreeTemplate* treeTmpl = mImpl->tree( treeName );
	Tree::Impl* treeImpl = NEW Tree::Impl( treeTmpl );
	treeImpl->release(); //コンテナは管理しない
	return Tree( treeImpl );
}

Tree Container::createTree( int treeId ) const {
	TreeTemplate* treeTmpl = mImpl->tree( treeId );
	Tree::Impl* treeImpl = NEW Tree::Impl( treeTmpl );
	treeImpl->release(); //コンテナは管理しない
	return Tree( treeImpl );
}

Batch Container::batch( const char* batchName ) const {
	Batch::Impl* batchImpl = mImpl->batch( batchName );
	return Batch( batchImpl );
}

Batch Container::batch( int batchId ) const {
	Batch::Impl* batchImpl = mImpl->batch( batchId );
	return Batch( batchImpl );
}

Animation Container::animation( const char* name ) const {
	Animation::Impl* impl = mImpl->animation( name );
	return Animation( impl );
}

Animation Container::animation( int index ) const {
	Animation::Impl* impl = mImpl->animation( index );
	return Animation( impl );
}

int Container::batchNumber() const {
	return mImpl->batchNumber();
}

int Container::treeNumber() const {
	return mImpl->treeNumber();
}

int Container::animationNumber() const {
	return mImpl->animationNumber();
}

bool Container::isReady(){
	return mImpl->isReady();
}

#define TYPE Container
#include "GameLib/Base/Impl/ReferenceTypeTemplate.h"

} //namespace Scene
} //namespace GameLib