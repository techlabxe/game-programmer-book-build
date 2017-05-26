#ifndef INCLUDED_SEQUENCE_TITLE_H
#define INCLUDED_SEQUENCE_TITLE_H

#include "Sequence/Child.h"

class Image;

namespace Sequence{
class Parent;

class Title : public Child{
public:
	Title();
	~Title();
	Base* update( Parent* );
private:
	Image* mImage; //ƒ^ƒCƒgƒ‹‰æ–Ê‰æ‘œ
	int mCursorPosistion;
};

} //namespace Sequence

#endif