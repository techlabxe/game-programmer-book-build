#ifndef INCLUDED_GRAPHICS_DATABASE_H
#define INCLUDED_GRAPHICS_DATABASE_H

namespace GameLib{
	namespace PseudoXml{
		class Element;
	}
}

class VertexBuffer;
class IndexBuffer;
class Texture;
class Batch;
class Model;
class Tree;
class TreeTemplate;
class Animation;

class GraphicsDatabase{
public:
	//ニセxmlのElementから生成
	GraphicsDatabase( GameLib::PseudoXml::Element& );
	//ファイルから生成
	GraphicsDatabase( const char* filename );
	~GraphicsDatabase();
	//取得系
	const VertexBuffer* vertexBuffer( const char* name ) const;
	const IndexBuffer* indexBuffer( const char* name ) const;
	const Texture* texture( const char* name ) const;
	const Batch* batch( const char* name ) const;
	const TreeTemplate* tree( const char* name ) const;
	const Animation* animation( const char* name ) const;
	//モデル生成
	Model* createModel( const char* batchName ) const;
	//ツリー生成
	Tree* createTree( const char* treeName ) const;
private:
	void createFromElement( GameLib::PseudoXml::Element& );

	VertexBuffer** mVertexBuffers;
	IndexBuffer** mIndexBuffers;
	Texture** mTextures;
	Batch** mBatches;
	TreeTemplate** mTrees;
	Animation** mAnimations;

	int mVertexBufferNumber;
	int mIndexBufferNumber;
	int mTextureNumber;
	int mBatchNumber;
	int mTreeNumber;
	int mAnimationNumber;
};

#endif

