#ifndef INCLUDED_GAMELIB_THREADING_MANAGER_H
#define INCLUDED_GAMELIB_THREADING_MANAGER_H

namespace GameLib{
namespace Threading{

class Manager{
public:
	Manager();
	static Manager instance();

	//コア数取得(論理コア数なので、実体とは違う可能性がある)
	int getCoreNumber() const;

	//以下ユーザが呼ぶことはない
	static void create( int additionalThreadNumber );
	static void destroy();
};

} //namespace Threading
} //namespace GameLib

#endif
