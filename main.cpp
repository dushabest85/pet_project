#include <iostream>

class Base
{
public:
    Base() {std::cout<<"Base"<<std::endl;}
    virtual ~Base() {std::cout<<"~Base"<<std::endl;};
};

class Derived1 : public Base
{
public:
    Derived1() {std::cout<<"Derived1"<<std::endl;}
    ~Derived1() {std::cout<<"~Derived1"<<std::endl;}
};

class Derived2 final : public Derived1
{
public:
    Derived2() {std::cout<<"Derived2"<<std::endl;}
    ~Derived2() {std::cout<<"~Derived2"<<std::endl;}
};

class A
{
public:
    A() {std::cout<<"A"<<std::endl;}
    virtual ~A() {std::cout<<"~A"<<std::endl;};
};

class B : virtual public A
{
public:
    B() {std::cout<<"B"<<std::endl;}
    ~B() {std::cout<<"~B"<<std::endl;}
};

class C : virtual public A
{
public:
    C() {std::cout<<"C"<<std::endl;}
    ~C() {std::cout<<"~C"<<std::endl;}
};

class D : public B, public C
{
public:
    D() {std::cout<<"D"<<std::endl;}
    ~D() {std::cout<<"~D"<<std::endl;}
};

int main()
{
    // Base *b = new Derived1;
    // delete b;

    A *a = new D;
    delete a;

    return 0;
}
