#include <iostream>
#include <string>
#include <Perturb/Perturb.h>
#include <Perturb/Application.h>
#include <Perturb/PartInterface.h>
#include <Perturb/ApplicationController.h>

class messageTest : public Perturb::ApplicationController
{

  protected:
  
  void Main(void * p)
  {
    std::string * s = static_cast<std::string*>(p);
    this->Interface().Log(*s);
    this->Interface().LogError(*s);
    this->Exit();
  }
};

int main()
{
  Perturb::Application app;
  messageTest mt;
  std::string s = "Hello World!";
  Perturb::Address addr = app.Execute(&mt, (void *)&s);
  
  if(addr == Theron::Address::Null())
    std::cout << "Error";
  app.WaitForQuit();
  return 0;
}
