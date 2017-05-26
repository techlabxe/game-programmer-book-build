#ifndef INCLUDED_GAMELIB_SCENE_CURVE_H
#define INCLUDED_GAMELIB_SCENE_CURVE_H

#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
#include "GameLib/Base/RefString.h"
#include "GameLib/Math/Functions.h"

namespace GameLib{
using namespace PseudoXml;
using namespace Math;
namespace Scene{

class Curve{
public:
	enum Type{
		TYPE_TX, //transX
		TYPE_TY, //transY
		TYPE_TZ, //transZ
		TYPE_RX, //rotateX
		TYPE_RY, //rotateY
		TYPE_RZ, //rotateZ
		TYPE_SX, //scaleX
		TYPE_SY, //scaleY
		TYPE_SZ, //scaleZ
		TYPE_SA, //scale ALL(XYZ)
		TYPE_UNKNOWN,
	};
	struct Key{
		Key() : mTime( 0.f ), mValue( 0.f ), mLeftSlope( 0.f ), mRightSlope( 0.f ){}
		float mTime;
		float mValue;
		float mLeftSlope;
		float mRightSlope;
	};
	Curve( PseudoXml::ConstElement& e ) :
	mKeys( 0 ),
	mKeyNumber( 0 ),
	mType( TYPE_UNKNOWN ), //適当
	mInterporation( INTERPORATION_NONE ){
		ASSERT( RefString( "Curve" ) == e.name() ); //Curveだよね？
		ASSERT( e.childNumber() > 0 ); //データ一個はあるよね？
		//名前取得
		bool isRot = false; //ファイルは度なので、ラジアンに直す。
		int an = e.attributeNumber();
		for ( int i = 0; i < an; ++i ){
			ConstAttribute a = e.attribute( i );
			RefString name( a.name() );
			RefString value( a.value() );
			if ( name == "type" ){
				if ( value == "translationX" ){
					mType = TYPE_TX;
				}else if ( value == "translationY" ){
					mType = TYPE_TY;
				}else if ( value == "translationZ" ){
					mType = TYPE_TZ;
				}else if ( value == "rotationX" ){
					mType = TYPE_RX;
					isRot = true;
				}else if ( value == "rotationY" ){
					mType = TYPE_RY;
					isRot = true;
				}else if ( value == "rotationZ" ){
					mType = TYPE_RZ;
					isRot = true;
				}else if ( value == "scaleX" ){
					mType = TYPE_SX;
				}else if ( value == "scaleY" ){
					mType = TYPE_SY;
				}else if ( value == "scaleZ" ){
					mType = TYPE_SZ;
				}else if ( value == "scaleXYZ" ){
					mType = TYPE_SA;
				}
			}else if ( name == "interporation" ){
				if ( value == "none" ){
					mInterporation = INTERPORATION_NONE;
				}else if ( value == "linear" ){
					mInterporation = INTERPORATION_LINEAR;
				}else if ( value == "cubic" ){
					mInterporation = INTERPORATION_CUBIC;
				}
			}
		}
		mKeyNumber = e.childNumber();
		mKeys = static_cast< Key* >( OPERATOR_NEW( sizeof( Key ) * mKeyNumber ) );
		for ( int i = 0; i < mKeyNumber; ++i ){
			new ( &mKeys[ i ] ) Key;
			ConstElement c = e.child( i );
			an = c.attributeNumber();
			for ( int j = 0; j < an; ++j ){
				ConstAttribute a = c.attribute( j );
				RefString name( a.name() );
				float v = a.getFloatValue();
				if ( name == "time" ){
					mKeys[ i ].mTime = v;
				}else if ( name == "value" ){
					mKeys[ i ].mValue = v;
				}else if ( name == "leftSlope" ){
					mKeys[ i ].mLeftSlope = v;
				}else if ( name == "rightSlope" ){
					mKeys[ i ].mRightSlope = v;
				}else if ( name == "slope" ){
					mKeys[ i ].mRightSlope = mKeys[ i ].mLeftSlope = v;
				}else{
					ASSERT( false ); //ありえん
				}
			}
		}
	}
	~Curve(){
		mName = 0; //コンテナが持っているので参照を外すだけ
		for ( int i = 0; i < mKeyNumber; ++i ){
			mKeys[ i ].~Key();
		}
		OPERATOR_DELETE( mKeys );
	}
	float operator()( float t ) const {
		//ループ処理をする。最後のデータの時刻が周期と考える。これで循環するね。
		//aから何回bが引けるかを計算するには、a -= toInt(a/b) * bで求まる。toIntは整数に切り捨てる関数としよう。キャストをうまくつかえば出来る。
		float quot = t / mKeys[ mKeyNumber - 1 ].mTime; //割ったものを整数に直すと商の整数が出る
		int quotInt = static_cast< int >( quot ); 
		t -= static_cast< float >( quotInt ) * mKeys[ mKeyNumber - 1 ].mTime;

		if ( mInterporation == INTERPORATION_NONE ){
			int last = 0;
			for ( int i = 0; i < mKeyNumber; ++i ){
				if ( mKeys[ i ].mTime > t ){
					break;
				}
				last = i;
			}
			//tを越えない最小のtimeを持つデータを使う(補間なし)
			return mKeys[ last ].mValue;
		}else if ( mInterporation == INTERPORATION_LINEAR ){
			int begin = 0;
			int end = 0;
			for ( end = 0; end < mKeyNumber; ++end ){
				if ( mKeys[ end ].mTime > t ){
					break;
				}
				begin = end;
			}
			if ( end >= mKeyNumber ){ //あふれ防止。最後の値返してやれ
				return mKeys[ mKeyNumber - 1 ].mValue;
			}
			float t0 = mKeys[ begin ].mTime;
			float t1 = mKeys[ end ].mTime;
			float p0 = mKeys[ begin ].mValue;
			float p1 = mKeys[ end ].mValue;
			//変数変換
			t = ( t - t0 ) / ( t1 - t0 );
			//線形補間
			return p0 + ( p1 - p0 ) * t;
		}else if ( mInterporation == INTERPORATION_CUBIC ){
			int begin = 0;
			int end = 0;
			for ( end = 0; end < mKeyNumber; ++end ){
				if ( mKeys[ end ].mTime > t ){
					break;
				}
				begin = end;
			}
			if ( end >= mKeyNumber ){ //あふれ防止。最後の値返してやれ
				return mKeys[ mKeyNumber - 1 ].mValue;
			}
			float t0 = mKeys[ begin ].mTime;
			float t1 = mKeys[ end ].mTime;
			float p0 = mKeys[ begin ].mValue;
			float p1 = mKeys[ end ].mValue;
			float v0 = mKeys[ begin ].mRightSlope; //始点の右傾き
			float v1 = mKeys[ end ].mLeftSlope; //終点の左傾き
			//変数変換
			t = ( t - t0 ) / ( t1 - t0 );
			//at^3 + bt^2 + c + dで計算。c=v0、d=p0だ。
			float a = 2.f * ( p0 - p1 ) + ( v0 + v1 );
			float b = 3.f * ( p1 - p0 ) - ( 2.f * v0 ) - v1;
			//線形補間
			float r = a; //a
			r *= t; //at
			r += b; //at+b
			r *= t; //at^2+bt
			r += v0; //at^2+bt+c
			r *= t; //at^3+bt^2+ct
			r += p0; //at^3+bt^2+ct+d
			return r;
		}else{
			ASSERT( false ); //ありえん
			return 0.0;
		}
	}
	Type type() const {
		return mType;
	}
private:
	enum Interporation{
		INTERPORATION_NONE, //補間なし
		INTERPORATION_LINEAR, //線形
		INTERPORATION_CUBIC, //3次
	};
	Key* mKeys;
	int mKeyNumber;
	const char* mName;
	Type mType;
	Interporation mInterporation;
};

} //namespace Scene
} //namespace GameLib

#endif

