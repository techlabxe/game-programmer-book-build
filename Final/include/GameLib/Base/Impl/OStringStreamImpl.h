#ifndef INCLUDED_GAMELIB_OSTRINGSTREAMIMPL_H
#define INCLUDED_GAMELIB_OSTRINGSTREAMIMPL_H

namespace GameLib{

template< class T > inline void OStringStream::write( const T& a ){
	const char* p = reinterpret_cast< const char* >( &a );
	write( p, sizeof( T ) );
}

} //namespace GameLib

#endif