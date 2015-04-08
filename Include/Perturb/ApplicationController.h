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

/*
 Date of Creation: April 6th, 2015
 Author: Robert C. Taylor
 */

#ifndef __PERTURB_INCLUDE_PERTURB_APPLICATIONCONTROLLER_H__
#define __PERTURB_INCLUDE_PERTURB_APPLICATIONCONTROLLER_H__

#include <Perturb/Perturb.h>
#include <Perturb/Part.h>

namespace Perturb {

class ApplicationController : public Perturb::Part
{
    
  public:
  virtual bool Initialize() {};
  bool __internal_controller_init(void * p);
  void * pointer_;
  protected:
    void Exit();

    void RequestExitRouter(const int& k);
    void Entry(const int& p);
    void SetPointer(void *p);
    virtual void RequestExit();
    virtual void Main(void * p) {};
    
};

};
#endif
