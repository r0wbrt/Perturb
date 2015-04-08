#include <Perturb/ApplicationController.h>
#include <Perturb/PartInterface.h>
#include <Perturb/Application.h>
namespace Perturb {

bool ApplicationController::__internal_controller_init(void *p)
{
  this->Interface().AddInputHandler(this, &ApplicationController::RequestExitRouter, "ApplicationController::RequestExit");
  this->Interface().AddInputHandler(this, &ApplicationController::Entry, "ApplicationController::Main");
  this->pointer_ = p;
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
void ApplicationController::RequestExit()
{
  this->Exit();
}

void ApplicationController::Entry(const int& p)
{
  this->Main(this->pointer_);
}


};
