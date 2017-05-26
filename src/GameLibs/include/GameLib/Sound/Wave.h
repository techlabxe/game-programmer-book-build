#ifndef INCLUDED_GAMELIB_SOUND_WAVE_H
#define INCLUDED_GAMELIB_SOUND_WAVE_H

namespace GameLib{
namespace Sound{

class Wave{
public:
	static Wave create( const char* filename );
	///波形直入力(8bit)
	static Wave create( const unsigned char* data, int size, int frequency, int channelNumber ); 
	///波形直入力(16bit)
	static Wave create( const short* data, int size, int frequency, int channelNumber );
	bool isReady(); //ロード終わった？(constでないのは中で処理するため)
	bool isError() const; //エラー起こってない？

	//以下ユーザは意識しない関数群
	Wave();
	Wave( const Wave& );
	~Wave();
	operator void*() const;
	Wave& operator=( const Wave& );
	void release();
	bool operator==( const Wave& ) const;
	bool operator!=( const Wave& ) const;
private:
	class Impl;
	friend class Player;
	Impl* mImpl;
};

} //namespace Sound
} //namespace GameLib

#endif
