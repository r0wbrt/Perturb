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
#include <Perturb/PartFactory.h>
#include <Perturb/Part.h>

namespace Perturb {
  std::atomic_flag PartFactory::part_list_lock_ = ATOMIC_FLAG_INIT;
  std::map<std::string, PartCreatorBase *> PartFactory::part_list_;
  Part * PartFactory::CreatePart(const std::string& name)
  {
    if(PartFactory::part_list_.count(name) == 0)
      return NULL;
      
    PartCreatorBase * b = PartFactory::part_list_[name];
    Part * ret =  b->CreatePart();
    ret->__internal_set_part_name(name);
    return ret;
  }
  void PartFactory::DeletePart(Part * part)
  {
    if(PartFactory::part_list_.count(part->__internal_get_part_name()) == 0)
      return ;
      
    PartCreatorBase * b = PartFactory::part_list_[part->__internal_get_part_name()];
    b->DestroyPart(part);
  }
  bool PartFactory::RegisterPart(const std::string& name, PartCreatorBase * creator)
  {
    PartFactory::part_list_[name] = creator;
    return true;
  }
  bool PartFactory::DeRegisterPart(const std::string& name)
  {
    if(PartFactory::part_list_.count(name) == 0)
      return false;
    
    PartCreatorBase * b = PartFactory::part_list_[name];
    if(b->GetNumberOfInstances()!= 0)
      return false;
      
    PartFactory::part_list_.erase(name);
    return true;
  }
  void PartFactory::Lock()
  {
    while (PartFactory::part_list_lock_.test_and_set(std::memory_order_acquire))
             {}
  }
  void PartFactory::UnLock()
  {
    PartFactory::part_list_lock_.clear(std::memory_order_release);
  }
};
