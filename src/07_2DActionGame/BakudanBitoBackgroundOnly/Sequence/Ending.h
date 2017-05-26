#ifndef INCLUDED_SEQUENCE_ENDING_H
#define INCLUDED_SEQUENCE_ENDING_H

class Image;

namespace Sequence{
class Parent;

class Ending{
public:
	Ending();
	~Ending();
	void update( Parent* );
private:
	Image* mImage;
	int mCount;
};

} //namespace Sequence

#endif
