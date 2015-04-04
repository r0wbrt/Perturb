#include <Perturb/Perturb.h>
#include <iostream>

class test : public Theron::Actor
{
  public:
  test(Theron::Framework & f) : Theron::Actor(f)
  {
    std::cout << "Theron Linked" <<std::endl;
  }
};

int main()
{
 Theron::Framework f;
 test test(f); 
 return 0;
}
