#ifndef INCLUDED_GAMELIB_REFSTRING_H
#define INCLUDED_GAMELIB_REFSTRING_H

//#include <string>

namespace GameLib{
//using namespace std;

//参照型文字列クラス。
/*!
参照しか持たないため、元のデータが破棄されれば非常に危険なことになる。
const char*のみでは不便な場合、NULL終端が不可能な場合で、
かつ性能が気になる場合に限って使うことを推奨する。
*/
class RefString{
public:
	RefString();
	RefString( const char*, int size );
	explicit RefString( const char* );
	~RefString();
	void set( const char*, int size );
	void operator=( const char* );
//	void operator=( const string& );
	char operator[]( int i ) const;
	const char* get() const;
	int size() const;
	void setSize( int );
	///lastは最後の文字そのもの。+1するな。last==-1だと始点だけ変える
	void clamp( int first, int last = -1 ); 
	int find( char ) const; //前から文字を探して位置を返す。見つからないと-1。
	int rfind( char ) const; //後から文字を探して位置を返す。見つからないと-1。
	///参照先文字列をコピーしてポインタを返す。自前でdeleteすること。
	char* copyOriginal() const;

	bool operator==( const RefString& ) const;
	bool operator==( const char* ) const;
//	bool operator==( const string& ) const;
	bool operator!=( const RefString& ) const;
	bool operator!=( const char* ) const;
//	bool operator!=( const string& ) const;
	bool operator<( const RefString& ) const;
private:
	const char* mString;
	int mSize;
};

} //namespace GameLib

#endif