#ifndef INCLUDED_GRAPHICS_DATABASE_H
#define INCLUDED_GRAPHICS_DATABASE_H

namespace GameLib{
	namespace PseudoXml{
		class Element;
	}
}
using namespace GameLib; //面倒なのでusing。あまりよくない。

class VertexBuffer;
class IndexBuffer;
class Texture;
class Batch;
class Model;

class GraphicsDatabase{
public:
	//ニセxmlのElementから生成
	GraphicsDatabase( GameLib::PseudoXml::Element& );
	//ファイルから生成
	GraphicsDatabase( const char* filename );
	~GraphicsDatabase();
	//取得系(const版で返す)
	ConstSharedPtr< VertexBuffer > getVertexBuffer( const char* name ) const;
	ConstSharedPtr< IndexBuffer > getIndexBuffer( const char* name ) const;
	ConstSharedPtr< Texture > getTexture( const char* name ) const;
	ConstSharedPtr< Batch > getBatch( const char* name ) const;
	//モデル生成
	Model* createModel( const char* batchName ) const;
private:
	void createFromElement( GameLib::PseudoXml::Element& );

	Array< SharedPtr< VertexBuffer > > mVertexBuffers;
	Array< SharedPtr< IndexBuffer > > mIndexBuffers;
	Array< SharedPtr< Texture > > mTextures;
	Array< SharedPtr< Batch > > mBatches;
};

#endif

