/*
   Copyright 2015 Robert C. Taylor

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifndef __PERTURB_INCLUDE_PERTURB_APPLICATION_H__
#define __PERTURB_INCLUDE_PERTURB_APPLICATION_H__

#include <Perturb/Perturb.h>
#include <Perturb/Part.h>
#include <Perturb/PartMessages.h>
#include <Perturb/PartInterface.h>
#include <map>
#include <atomic>

namespace Perturb {

class ApplicationController;

class Application
{
  private:
  Theron::Framework framework_;
  Theron::Receiver receiver_;
  std::map<Perturb::Address, std::pair<Part *, PartInterface *> > part_map_;
  std::atomic_flag lock_ = ATOMIC_FLAG_INIT;
  static std::atomic_flag stdout_lock_;
  static std::atomic_flag stderror_lock_;
  bool has_quit_ = true;
  Part * application_controller_;
  PartInterface * application_controller_interface_;
  
  void QuitCatcher(const PartInterfaceInputMessage<int>& message, Perturb::Address address);
  void OutputCatcher(const PartInterfaceInputMessage<std::string>& message, const Perturb::Address address);
  void Lock();
  void Unlock();
  
  public:
  Application();
  Perturb::Address AddPart(Part * part, Part * parent);
  Part * RemovePart(Perturb::Address address);
  
  Perturb::Address Execute(ApplicationController * controller, void * p);
  /*Has the application controller initiated a shutdown?*/
  bool HasQuit();
  /*Informs the application controller that is should shutdown*/
  bool RequestQuit();
  /*Returns after the Application Controller has shutdown Application*/
  bool WaitForQuit();
  Perturb::Address GetAddress();
};

};

#endif
