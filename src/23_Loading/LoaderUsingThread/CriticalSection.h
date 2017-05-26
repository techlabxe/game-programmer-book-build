#ifndef INCLUDED_CRITICAL_SECTION_H
#define INCLUDED_CRITICAL_SECTION_H

//クリティカルセクションクラス
class CriticalSection{
public:
	CriticalSection();
	void enter();
	void leave();
private:
	int mEntered;
};

#endif
