#include <iostream>
using namespace std;

class A{
public:
	A() : mTypeName( 'A' ){}
	void foo(){ cout << "a" << endl; }
	char mTypeName;
};

class B{
public:
	B() : mTypeName( 'B' ){}
	void foo(){ cout << "b" << endl; }
	char mTypeName;
};

class C{
public:
	C() : mTypeName( 'C' ){}
	void foo(){ 
		if ( mTypeName == 'A' ){
			A* a = ( A* )this;
			a->foo();
		}else if ( mTypeName == 'B' ){
			B* b = ( B* )this;
			b->foo();
		}else{
			cout << "c" << endl; 
		}
	}
	char mTypeName;
};

int main(){
	C* c0 = ( C* )new A();
	C* c1 = ( C* )new B();
	c0->foo();
	c1->foo();

	while ( true ); //–³ŒÀƒ‹[ƒv
	return 0;
}
