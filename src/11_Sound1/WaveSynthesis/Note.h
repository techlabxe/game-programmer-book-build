#ifndef INCLUDED_NOTE_H
#define INCLUDED_NOTE_H

struct Note{
public:
	int mName; //ドレミ。0が65Hzのド。24がいわゆる真ん中のド。
	int mLength; // 100分の1秒単位
	int mVolume; // 0から100
};

#endif
