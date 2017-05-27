#ifndef INCLUDED_GAMELIB_THREADING_EVENT_H
#define INCLUDED_GAMELIB_THREADING_EVENT_H

namespace GameLib{
namespace Threading{

class Event{
public:
	static Event create( bool initialiState = false );
	///true‚É‚³‚ê‚é‚Ì‚ð‘Ò‚Â
	void wait() const;
	///true‚É‚·‚é
	void set();
	///false‚É‚·‚é
	void reset();
	///true?‚»‚ê‚Æ‚àfalse?
	bool isSet() const;

	Event();
	~Event();
	Event( const Event& );
	operator void*() const;
	Event& operator=( const Event& );
	void release();
	bool operator==( const Event& ) const;
	bool operator!=( const Event& ) const;
private:
	class Impl;
	Impl* mImpl;
};

} //namespace Threading
} //namespace GameLib

#endif
