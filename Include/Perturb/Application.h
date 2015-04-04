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

namespace Perturb {

class ApplicationController;

class Application
{
  private:
  Theron::Framework framework_;
  std::map<Perturb::Address, std::pair<Part *, PartInterface *> > part_map_;
  Theron::Receiver receiver_;
  std::atomic_flag lock_ = ATOMIC_FLAG_INIT;
  ContextToken default_token_ = 0;
  public:
  /*Creates a new part*/
  Perturb::Address CreatePart(const std::string& name);
  /*Destroys a part*/
  bool DestroyPart(const Perturb::Address address);
  /*Executes the ApplicationController of the app*/
  bool Execute(ApplicationController * controller, void * p);
  /*Creates a que with type T to catch replies from the AppController*/
  template <typename T>
  bool CreateQue();
  /*Waits for a message from the ApplicationController*/
  bool Wait();
  
  bool Link(Perturb::Address source_part, Perturb::Address sink_part, 
        const std::string& output_name, 
        const std::string& input_name, TypeHash type);
        
  bool Link(Perturb::Address source_part, Perturb::Address sink_part, 
            const NameHash output_hash, 
            const NameHash input_hash, TypeHash type);
            
  bool UnLink(Perturb::Address source_part, Perturb::Address sink_part, 
              const std::string& output_name, 
              const std::string& input_name,  TypeHash type);
              
  bool UnLink(Perturb::Address source_part, Perturb::Address sink_part, 
            const NameHash output_hash, 
            const NameHash input_hash, TypeHash type);
  
  template <typename T>
  bool ReadQue(T& value); 
  
  /*Has the application controller intiated a shutdown?*/
  bool HasQuit();
  /*Sends value with type n to the application controller*/
  template <typename T>
  bool Send(T& n);
  /*Informs the application controller that is should shutdown*/
  bool RequestQuit();
  /*Returns after the Application Controller has shutdown Application*/
  bool WaitForQuit();
  
  void SetDefaultToken(ContextToken token);
  ContextToken GetDefaultToken();
};

};

#endif
