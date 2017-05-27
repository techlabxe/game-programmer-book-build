#ifndef INCLUDED_GAMELIB_THREADING_MUTEX_H
#define INCLUDED_GAMELIB_THREADING_MUTEX_H

namespace GameLib{
namespace Threading{

class Mutex{
public:
	static Mutex create();
	void lock();
	void unlock();

	Mutex();
	~Mutex();
	Mutex( const Mutex& );
	operator void*() const;
	Mutex& operator=( const Mutex& );
	void release();
	bool operator==( const Mutex& ) const;
	bool operator!=( const Mutex& ) const;
private:
	class Impl;
	Impl* mImpl;
};

} //namespace Threading
} //namespace GameLib

#endif
