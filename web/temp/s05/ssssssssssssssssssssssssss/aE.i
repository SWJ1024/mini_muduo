# 1 "a.cc"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "a.cc"
# 1 "a.h" 1






class B;

class A{
public:
 A(){}
 void ff();
 B* b;

};
# 2 "a.cc" 2
# 1 "b.h" 1






class A;

class B{
public:
 B() {}
 void f();
 A* a;

};
# 3 "a.cc" 2


void A::ff() {
 b = new B();

 b->f();

}
