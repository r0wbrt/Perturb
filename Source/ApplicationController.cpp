#include <Perturb/ApplicationController.h>

namespace Perturb {

void Initialize()
{
  this->Interface().AddInputHandler(this, &ApplicationController::RequestExitRouter, "ApplicationController::RequestExit");
  this->Interface().AddInputHandler(this, &ApplicationController::Entry, "ApplicationController::Main");
  return true;
}
void ApplicationController::Exit()
{
  this->Interface().SendToInput<int>(0, 0, 0, 0, this->App().GetAddress());
}
void ApplicationController::RequestExitRouter(const int& k)
{
  if(k!=0xDEADBEEF)
    return ;
    
  if(this->GetFromOutput()!=PartInterface::HashName("Application::RequestExit"))
    return ;
    
  this->RequestExit();
}
void Application::RequestExit()
{
  this->Exit();
}
void Application::Entry(const void *& p)
{
  this->Main(p);
}


};
