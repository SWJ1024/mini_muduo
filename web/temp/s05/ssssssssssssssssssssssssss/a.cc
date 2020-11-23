#include "a.h"
#include "b.h"
#include <stdio.h>

void A::ff() {
	vec.resize(4);
//	b = new B();
	printf("i am A\n");
	vec[0]->f();
	printf("i am A\n");
}
