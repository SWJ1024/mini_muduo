#ifndef A_H
#define A_H

#include <vector>
#include <memory>
class B;

class A{
public:
//	A(){}
	void ff();
	B* b;
	std::vector<std::unique_ptr<B>> vec;
};


#endif
