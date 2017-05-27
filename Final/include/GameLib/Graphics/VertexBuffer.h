#ifndef INCLUDED_GAMELIB_GRAPHICS_VERTEXBUFFER_H
#define INCLUDED_GAMELIB_GRAPHICS_VERTEXBUFFER_H

namespace GameLib{
namespace PseudoXml{
	class ConstElement;
}
namespace Graphics{

class Vertex;

class VertexBuffer{
public:
	static VertexBuffer create( PseudoXml::ConstElement );
	static VertexBuffer create( int vertexNumber );
	Vertex* lock();
	void unlock( Vertex** );
	const char* name() const;
	int vertexNumber() const;

	//à»â∫ÉÜÅ[ÉUÇÕà”éØÇµÇ»Ç¢ä÷êîåQ
	VertexBuffer();
	VertexBuffer( const VertexBuffer& );
	~VertexBuffer();
	operator void*() const;
	VertexBuffer& operator=( const VertexBuffer& );
	void release();
	bool operator==( const VertexBuffer& ) const;
	bool operator!=( const VertexBuffer& ) const;

	class Impl;
	VertexBuffer( const Impl* );
private:
	friend class Manager;
	Impl* mImpl;
};

} //namespace Graphics
} //namespace GameLib

#endif
