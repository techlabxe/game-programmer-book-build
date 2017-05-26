#ifndef INCLUDED_GAMELIB_PSEUDOXML_ATTRIBUTE_H
#define INCLUDED_GAMELIB_PSEUDOXML_ATTRIBUTE_H

namespace GameLib{
namespace PseudoXml{

class Attribute{
public:
	///NULL終端でないなどしてサイズを明示的に与えたい時。-1を入れれば勝手に測るので、片方だけ与えることもできる
	void set( const char* name, int nameSize, const char* value, int valueSize );
	void set( const char* name, const char* value );
	void set( const char* name, int value ); //int値
	void set( const char* name, float value ); //float値
	void set( const char* name, double value ); //double値
	void set( const char* name, const int* values, int number ); //int配列
	void set( const char* name, const float* values, int number ); //float配列
	void set( const char* name, const double* values, int number ); //double配列
	const char* value() const;
	const char* name() const;
	///失敗すると0を返すが、もともと0が入っていたのとは区別がつかない。
	int getIntValue() const;
	///失敗すると0.fを返すが、もともと0.fが入っていたのとは区別がつかない。
	float getFloatValue() const;
	///失敗すると0.0を返すが、もともと0.0が入っていたのとは区別がつかない。
	double getDoubleValue() const;
	///戻り値は実際に読めた要素数
	int getIntValues( int* out, int number ) const;
	///戻り値は実際に読めた要素数
	int getFloatValues( float* out, int number ) const;
	///戻り値は実際に読めた要素数
	int getDoubleValues( double* out, int number ) const;

	//以下ユーザは意識しなくていい
	Attribute();
	~Attribute();
	Attribute( const Attribute& );
	operator void*() const;
	Attribute& operator=( const Attribute& );
	void release();
	bool operator==( const Attribute& ) const;
	bool operator!=( const Attribute& ) const;

	class Impl;
private:
	friend class Element;
	friend class Tag;

	explicit Attribute( Impl* );
	Impl* mImpl;
};

//const版
class ConstAttribute{
public:
	const char* value() const;
	const char* name() const;
	///失敗すると0を返すが、もともと0が入っていたのとは区別がつかない。
	int getIntValue() const;
	///失敗すると0.fを返すが、もともと0.fが入っていたのとは区別がつかない。
	float getFloatValue() const;
	///失敗すると0.0を返すが、もともと0.0が入っていたのとは区別がつかない。
	double getDoubleValue() const;
	///戻り値は実際に読めた要素数
	int getIntValues( int* out, int number ) const;
	///戻り値は実際に読めた要素数
	int getFloatValues( float* out, int number ) const;
	///戻り値は実際に読めた要素数
	int getDoubleValues( double* out, int number ) const;

	//以下ユーザは意識しなくていい
	ConstAttribute();
	ConstAttribute( const Attribute& );
	~ConstAttribute();
	operator void*() const;
	ConstAttribute& operator=( const Attribute& );
	void release();
	bool operator==( const Attribute& ) const;
	bool operator!=( const Attribute& ) const;
private:
	Attribute mBase;
};


} //namespace PseudoXml
} //namespace GameLib

#endif
