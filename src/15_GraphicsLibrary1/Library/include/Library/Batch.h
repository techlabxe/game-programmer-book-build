#ifndef INCLUDED_BATCH_H
#define INCLUDED_BATCH_H

#include "GameLib/Framework.h"

class VertexBuffer;
class IndexBuffer;
class Matrix44;

class Batch{
public:
	Batch( 
		const VertexBuffer*,
		const IndexBuffer*,
		const GameLib::Texture*,
		GameLib::Framework::BlendMode );
	~Batch();
	void draw( const Matrix44& transform ) const;
private:
	void operator=( const Batch& ); //‘ã“ü••ˆó

	const VertexBuffer* mVertexBuffer;
	const IndexBuffer* mIndexBuffer;
	const GameLib::Texture* mTexture;
	const GameLib::Framework::BlendMode mBlendMode;
};

#endif
