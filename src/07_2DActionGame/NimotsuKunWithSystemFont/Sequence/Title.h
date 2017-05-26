#ifndef INCLUDED_SEQUENCE_TITLE_H
#define INCLUDED_SEQUENCE_TITLE_H

class Image;

namespace Sequence{
class Parent;

class Title{
public:
	Title();
	~Title();
	void update( Parent* );
private:
	Image* mImage; //ƒ^ƒCƒgƒ‹‰æ–Ê‰æ‘œ
};

} //namespace Sequence

#endif