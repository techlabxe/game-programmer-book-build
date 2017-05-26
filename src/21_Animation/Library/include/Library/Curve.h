#ifndef INCLUDED_CURVE_H
#define INCLUDED_CURVE_H

namespace GameLib{
	namespace PseudoXml{
		class Element;
	}
}

class Curve{
public:
	enum Type{
		TYPE_TRANSLATION_X,
		TYPE_TRANSLATION_Y,
		TYPE_TRANSLATION_Z,
		TYPE_ROTATION_X,
		TYPE_ROTATION_Y,
		TYPE_ROTATION_Z,
		TYPE_SCALE_X,
		TYPE_SCALE_Y,
		TYPE_SCALE_Z,
	};
	enum Interporation{
		INTERPORATION_NONE, //補間なし。
		INTERPORATION_LINEAR, //1次補間
		INTERPORATION_CUBIC, //3次補間
	};
	Curve( GameLib::PseudoXml::Element& );
	~Curve();
	//ある時刻のデータをもらう
	double get( double time ) const;
	//タイプ取得
	Type type() const;
private:
	struct Data{
		Data();
		double mTime;
		double mValue;
		double mLeftSlope;
		double mRightSlope;
	};
	Data* mData;
	int mDataNumber;
	Type mType;
	Interporation mInterporation;
};

#endif
