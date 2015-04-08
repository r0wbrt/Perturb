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
#include <iostream>
#include <Perturb/Perturb.h>
#include <Perturb/Application.h>
#include <Perturb/ApplicationController.h>

namespace Perturb 
{
  std::atomic_flag Application::stdout_lock_ = ATOMIC_FLAG_INIT;
  std::atomic_flag Application::stderror_lock_ = ATOMIC_FLAG_INIT;

  Application::Application()
  {
    this->receiver_.RegisterHandler(this, &Application::QuitCatcher);
    this->receiver_.RegisterHandler(this, &Application::OutputCatcher);
  }

  void Application::QuitCatcher(const PartInterfaceInputMessage<int>& message, 
    const Perturb::Address address)
  {
    if(address != this->application_controller_interface_->GetAddress())
      return ;
      
    if(message.input_hash != 0 && message.output_hash != 0)
      return ;
      
    this->has_quit_ = true;
  }

  void Application::OutputCatcher(const PartInterfaceInputMessage<std::string>& message, 
        const Perturb::Address address)
  {

    if(message.input_hash == PartInterface::HashName("Application::stdout"))
    {
      while(Application::stdout_lock_.test_and_set(std::memory_order_acquire))
          {}
          
      std::cout << "Output from " << address.AsString() << ": " << message.payload << std::endl;
      
      Application::stdout_lock_.clear(std::memory_order_release);
   }
   else if(message.input_hash == PartInterface::HashName("Application::stderror"))
   {
      while(Application::stderror_lock_.test_and_set(std::memory_order_acquire))
          {}
          
      std::cerr <<"Error from " << address.AsString() << ": " << message.payload << std::endl;
      
      Application::stderror_lock_.clear(std::memory_order_release);
   }    
  }
  void Application::Lock()
  {
    while (this->lock_.test_and_set(std::memory_order_acquire))
    {}
  }
  void Application::Unlock()
  {
    this->lock_.clear(std::memory_order_release);
  }

  Perturb::Address Application::AddPart(Part * part, Part * parent)
  {
  
    PartInterface * pi = new Perturb::PartInterface(this->framework_);
    if(pi == NULL)
    {
      return Perturb::Address::Null();
    }
    
    pi->Initialize(part, this->receiver_.GetAddress());
    part->__internal_initialize(pi);
    part->__internal_set_check_token(parent->__internal_get_check_token());
    part->CheckTokens(true);
    part->Initialize();
    part->__internal_set_application(this);
    
    this->Lock();
    
    std::pair<Part *, PartInterface *> entry(part, pi);
    this->part_map_[pi->GetAddress()] = entry;
    
    this->Unlock();
    
    return pi->GetAddress();
  }
  
  Part * Application::RemovePart(Perturb::Address address)
  {
  
    this->Lock();
    
    if(this->part_map_.count(address) == 0)
      return NULL;
      
    std::pair<Part *, PartInterface *> entry = this->part_map_[address];
    this->part_map_.erase(address);
    
    this->Unlock();
    
    Part * part = entry.first;
    PartInterface * pi = entry.second;
    
    delete pi;
    return part;
  }
  
  Perturb::Address Application::Execute(ApplicationController * controller, void * p)
  {
    PartInterface * pi = new Perturb::PartInterface(this->framework_);
    if(pi == NULL)
    {
      return Perturb::Address::Null();
    }
    
    pi->Initialize(static_cast<Part *>(controller), this->receiver_.GetAddress());
    
    controller->__internal_set_application(this);
    controller->__internal_initialize(pi);
    controller->__internal_controller_init(p);
    controller->__internal_set_check_token(0);
    controller->CheckTokens(false);
    controller->Initialize();
    
    this->application_controller_ = controller;
    this->application_controller_interface_ = pi;
    
    PartInterfaceInputMessage<int> message;
    message.type = typeid(int).hash_code();
    message.input_hash = PartInterface::HashName("ApplicationController::Main");
    message.output_hash = PartInterface::HashName("Application::Execute");
    message.payload = 0;
    message.token = 0;
    this->framework_.Send(message, this->receiver_.GetAddress(), 
                           pi->GetAddress());
    this->has_quit_ = false;
    
    return pi->GetAddress();
  }

  bool Application::HasQuit()
  {
    return this->has_quit_;
  }
  bool Application::RequestQuit()
  {
    PartInterfaceInputMessage<int> message;
    message.type = typeid(int).hash_code();
    message.input_hash = PartInterface::HashName("ApplicationController::RequestExit");
    message.output_hash = PartInterface::HashName("Application::RequestExit");
    message.payload = 0xDEADBEEF;
    message.token = 0;
    this->framework_.Send(message, this->receiver_.GetAddress(), 
      this->application_controller_interface_->GetAddress());
  }
  bool Application::WaitForQuit()
  {
    while(this->has_quit_ != true)
      this->receiver_.Wait();
      
    return true;
  }
  Perturb::Address Application::GetAddress()
  {
    return this->receiver_.GetAddress();
  }

};
