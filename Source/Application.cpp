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

#include <Perturb/Perturb.h>
#include <Perturb/Application.h>
#include <Perturb/PartFactory.h>

namespace Perturb 
{

  Perturb::Address Application::CreatePart(const std::string& name)
  {
    Part * part = PartFactory::CreatePart(name);
    if(part != NULL)
      Perturb::Address::Null();
    
    PartInterface * pi = new Perturb::PartInterface(this->framework_);
      
    pi->Initialize(part, this->receiver_.GetAddress());
    part->__internal_initialize(pi);
    part->__internal_set_check_token(this->default_token_);
    part->CheckTokens(true);
    part->Initialize();
    std::pair<Part *, PartInterface *> entry(part, pi);
    this->part_map_[pi->GetAddress()] = entry;
    return pi->GetAddress();
  }
  
  bool Application::DestroyPart(const Perturb::Address address)
  {
    if(this->part_map_.count(address) == 0)
      return false;
      
    std::pair<Part *, PartInterface *> entry = this->part_map_[address];
    this->part_map_.erase(address);
    Part * part = entry.first;
    PartInterface * pi = entry.second;
    PartFactory::DeletePart(part);
    
    delete pi;
    return true;
  }
  
  void Application::SetDefaultToken(ContextToken token)
  {
    this->default_token_ = token;
  }
  ContextToken Application::GetDefaultToken()
  {
    return this->default_token_;
  }
  
  bool Application::Execute(ApplicationController * controller, void * p){}
  /*
  template <typename T>
  bool CreateQue();
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
  

  bool HasQuit();
  bool RequestQuit();
  bool WaitForQuit();

*/
};
