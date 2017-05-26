#ifndef INCLUDED_GAMELIB_PSEUDOXML_ATTRIBUTEIMPL_H
#define INCLUDED_GAMELIB_PSEUDOXML_ATTRIBUTEIMPL_H

#include "GameLib/GameLib.h"
#include "GameLib/PseudoXml/Attribute.h"
#include "GameLib/Base/Pool.h"
#include "GameLib/Base/OStringStream.h"
#include "GameLib/Base/Conversion.h"
#include "PseudoXml/DocumentImpl.h"

namespace GameLib{
namespace PseudoXml{

class Attribute::Impl{
public:
	Impl() : 
	mName( 0 ),
	mValue( 0 ),
	mDocument( 0 ){
	}
	~Impl(){
		mName = mValue = 0;
		mDocument = 0;
	}
	void setDocument( Document::Impl* document ){
		mDocument = document;
	}
	void setNameReference( const char* name ){
		mName = name;
	}
	void setValueReference( const char* value ){
		mValue = value;
	}
	void setName( const char* name ){
		mName = mDocument->allocateString( name );
	}
	void setValue( const char* value ){
		mValue = mDocument->allocateString( value );
	}
	void setName( const string& name ){
		mName = mDocument->allocateString( name );
	}
	void setValue( const string& value ){
		mValue = mDocument->allocateString( value );
	}
	void setName( const char* name, int size ){
		mName = mDocument->allocateString( name, size );
	}
	void setValue( const char* value, int size ){
		mValue = mDocument->allocateString( value, size );
	}
	void setValue( int value ){
		char s[ 11 ];
		int c = toString10( s, value );
		ASSERT( c < 11 );
		mValue = mDocument->allocateString( s, c );
	}
	void setValue( float value ){
		char s[ 48 ];
		int c = toString( s, value, 8 );
		ASSERT( c < 48 );
		mValue = mDocument->allocateString( s, c  );
	}
	void setValue( double value ){
		char s[ 320 ];
		int c = toString( s, value, 17 );
		ASSERT( c < 320 );
		mValue = mDocument->allocateString( s, c );
	}
	void setValue( const int* values, int n ){
		if ( n == 0 ){
			return;
		}
		if ( n < 8 ){ //この範囲ならその場でやる
			char s[ 13*8 ];
			int p = 0;
			for ( int i = 0; i < ( n - 1 ); ++i ){
				p += toString10( s + p, values[ i ] );
				s[ p++ ] = ',';
				s[ p++ ] = ' ';
			}
			p += toString10( s + p, values[ n - 1 ] );
			s[ p++ ] = '\0';
			ASSERT( p < 13*8 );
			mValue = mDocument->allocateString( s, p );
		}else{
			OStringStream oss;
			if ( n > 0 ){
				for ( int i = 0; i < ( n - 1 ); ++i ){
					oss << values[ i ] << ", ";
				}
				oss << values[ n - 1 ];
			}
			Array< char > t;
			oss.get( &t );
			mValue = mDocument->allocateString( &t[ 0 ], t.size() );
		}
	}
	void setValue( const float* values, int n ){
		if ( n == 0 ){
			return;
		}
		if ( n < 8 ){ //この範囲ならその場でやる
			char s[ 16*8 ];
			int p = 0;
			for ( int i = 0; i < ( n - 1 ); ++i ){
				p += toString( s + p, values[ i ], 4 );
				s[ p++ ] = ',';
				s[ p++ ] = ' ';
			}
			p += toString( s + p, values[ n - 1 ], 4 );
			s[ p++ ] = '\0';
			ASSERT( p < 16*8 );
			mValue = mDocument->allocateString( s, p );
		}else{
			OStringStream oss;
			if ( n > 0 ){
				for ( int i = 0; i < ( n - 1 ); ++i ){
					oss << values[ i ] << ", ";
				}
				oss << values[ n - 1 ];
			}
			Array< char > t;
			oss.get( &t );
			mValue = mDocument->allocateString( &t[ 0 ], t.size() );
		}
	}
	void setValue( const double* values, int n ){
		if ( n == 0 ){
			return;
		}
		if ( n < 8 ){ //この範囲ならその場でやる
			char s[ 30*8 ];
			int p = 0;
			for ( int i = 0; i < ( n - 1 ); ++i ){
				p += toString( s + p, values[ i ], 17 );
				s[ p++ ] = ',';
				s[ p++ ] = ' ';
			}
			p += toString( s + p, values[ n - 1 ], 17 );
			s[ p++ ] = '\0';
			ASSERT( p < 30*8 );
			mValue = mDocument->allocateString( s, p );
		}else{
			OStringStream oss;
			if ( n > 0 ){
				for ( int i = 0; i < ( n - 1 ); ++i ){
					oss << values[ i ] << ", ";
				}
				oss << values[ n - 1 ];
			}
			Array< char > t;
			oss.get( &t );
			mValue = mDocument->allocateString( &t[ 0 ], t.size() );
		}
	}
	const char* name() const {
		return mName;
	}
	const char* value() const {
		return mValue;
	}
	int getIntValue() const {
		return atoi( mValue );
	}
	float getFloatValue() const {
		return static_cast< float >( atof( mValue ) );
	}
	double getDoubleValue() const {
		return atof( mValue );
	}
	static bool isIntStartChar( char c ){
		if ( c >= '0' && c <= '9' ){
			return true;
		}else if ( ( c == '+' ) || ( c == '-' ) ){
			return true;
		}else{
			return false;
		}
	}
	int getIntValues( int* a, int n ) const {
		int c = 0;
		char* endP;
		const char* p = mValue;
		for ( int i = 0; i < n; ++i ){
			//数字が出てくるまで読み飛ばす
			while ( !isIntStartChar( *p ) ){
				if ( *p == '\0' ){
					return c;
				}
				++p;
			}
			//変換本体
			a[ i ] = strtol( p, &endP, 10 );
			if ( endP == p ){ //進んでない。失敗した。
				return c;
			}
			p = endP;
			++c; //変換カウンタ+1
			//カンマまで読み飛ばす
			while ( *p != ',' ){
				if ( *p == '\0' ){
					return c;
				}
				++p;
			}
			++p; //カンマ本体も飛ばす
		}
		return c;
	}
	static bool isFloatStartChar( char c ){
		if ( c >= '0' && c <= '9' ){
			return true;
		}else if ( ( c == '+' ) || ( c == '-' ) || ( c == '.' ) ){
			return true;
		}else{
			return false;
		}
	}
	int getFloatValues( float* a, int n ) const {
		int c = 0;
		char* endP;
		const char* p = mValue;
		for ( int i = 0; i < n; ++i ){
			//数字が出てくるまで読み飛ばす
			while ( !isFloatStartChar( *p ) ){
				if ( *p == '\0' ){
					return c;
				}
				++p;
			}
			//変換本体
			a[ i ] = static_cast< float >( strtod( p, &endP ) );
			if ( endP == p ){ //進んでない。失敗した。
				return c;
			}
			p = endP;
			++c; //変換カウンタ+1
			//カンマまで読み飛ばす
			while ( *p != ',' ){
				if ( *p == '\0' ){
					return c;
				}
				++p;
			}
			++p; //カンマ本体も飛ばす
		}
		return c;
	}
	int getDoubleValues( double* a, int n ) const {
		int c = 0;
		char* endP;
		const char* p = mValue;
		for ( int i = 0; i < n; ++i ){
			//数字が出てくるまで読み飛ばす
			while ( !isFloatStartChar( *p ) ){
				if ( *p == '\0' ){
					return c;
				}
				++p;
			}
			//変換本体
			a[ i ] = strtod( p, &endP );
			if ( endP == p ){ //進んでない。失敗した。
				return c;
			}
			p = endP;
			++c; //変換カウンタ+1
			//カンマまで読み飛ばす
			while ( *p != ',' ){
				if ( *p == '\0' ){
					return c;
				}
				++p;
			}
			++p; //カンマ本体も飛ばす
		}
		return c;
	}
	Document::Impl* document(){
		return mDocument;
	}
private:
	const char* mName;
	const char* mValue;
	Document::Impl* mDocument;
};

} //namespace PseudoXml
} //namespace GameLib

#endif
