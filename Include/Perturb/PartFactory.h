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
#ifndef __PERTURB_INCLUDE_PERTURB_PARTFACTORY_H__
#define __PERTURB_INCLUDE_PERTURB_PARTFACTORY_H__

#include <Perturb/Perturb.h>
#include <map>
#include <string>
#include <atomic>

namespace Perturb {

class Part;

  class PartCreatorBase
  {
    public:
    virtual Part * CreatePart() = 0;
    virtual int GetNumberOfInstances();
    virtual void DestroyPart(Part * part) = 0;
  };
  template <typename T>
  class PartCreator : public PartCreatorBase
  {
    int count = 0;
    public:
    Part * CreatePart()
    {
      this->count++;
      return new T;
    }
    void DestroyPart(Part * part)
    {
      this->count--;
      delete part;
    }
    int GetNumberOfInstances()
    {
      return this->count;
    }
  };

  class PartFactory
  {
    private:
    static std::atomic_flag part_list_lock_;
    static std::map<std::string, PartCreatorBase *> part_list_;
    public:
    static Part * CreatePart(const std::string& name);
    static void DeletePart(Part * part);
    static bool RegisterPart(const std::string& name, PartCreatorBase * creator);
    static bool DeRegisterPart(const std::string& name);
    static void Lock();
    static void UnLock();
  };

};

#endif
