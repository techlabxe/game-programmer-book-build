#ifndef INCLUDED_SEQUENCE_BASE_H
#define INCLUDED_SEQUENCE_BASE_H

namespace Sequence{

class Base{
public:
	virtual ~Base(){} //‚È‚É‚à‚µ‚È‚¢‚Ì‚Å‚±‚ê‚Å‚¢‚¢
	virtual Base* update( Base* ) = 0;
};

} //namespace Sequence

#endif