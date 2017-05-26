#ifndef INCLUDED_BITSTREAM_H
#define INCLUDED_BITSTREAM_H

//読み込み専用
class IBitStream{
public:
	//領域をセットしてコンストラクト
	explicit IBitStream( const unsigned char* buffer ); //explicitは引数が一個のコンストラクタには必ずつけるおまじない。これを説明するスペースはないが、知らないとまずいので調べておくこと。
	void read( unsigned char* data, int sizeInBit ); //指定のバイト数読み込む
	bool read(); //1ビット読み込んで、1ならtrueを返す。
	unsigned readU4(); //unsigned int読み込み
	int readS4(); //signed int読み込み
	int positionInByte() const; //バイトで位置を返す。
private:
	const unsigned char* mBuffer;
	int mPosition; //ビット単位です。
};

//書き込み専用
class OBitStream{
public:
	//領域をセットしてコンストラクト
	explicit OBitStream( unsigned char* buffer ); //explicitは引数が一個のコンストラクタには必ずつけるおまじない。これを説明するスペースはないが、知らないとまずいので調べておくこと。
	void write( const unsigned char* data, int sizeInBit );//指定のビット数書き込む
	void write( bool ); //1ビット書き込み
	void write( unsigned ); //unsigned読み込み
	void write( int ); //int読み込み
	void add(); //最後の桁に1を足す。これはえんえん繰り上がっていく。
	int sizeInByte() const; //バイトでサイズを返す。
private:
	unsigned char* mBuffer;
	int mPosition; //ビット単位です。
};

#endif

