#ifndef INCLUDED_GAMELIB_THREADING_THREAD_H
#define INCLUDED_GAMELIB_THREADING_THREAD_H

namespace GameLib{
namespace Threading{

///スレッド抽象基底
/*!
operator()()を用意した派生を作り、
start()すれば実行が始まる。

<注意！>
デストラクタを呼ぶ前にはwait()を呼ぶか、isFinished()がtrueを返すまで呼ぶかの
どちらかが必要。どちらもやらないでデストラクトするとASSERTする。
*/
class Thread{
public:
	///これを自分で作る
	virtual void operator()() = 0;
	///実行開始。実行中なら終了を待って再実行。
	void start();
	///終了を待つ
	void wait();
	///終わったか聞く
	bool isFinished();
	///直接呼びはしないコンストラクタ
	Thread();
protected: //基底は直接deleteできません。派生をdeleteしましょう。
	~Thread();
private:
	class Impl;
	Impl* mImpl;
};

} //namespace Threading
} //namespace GameLib

#endif
