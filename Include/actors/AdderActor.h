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


#ifndef __PERTURB_INCLUDE_ACTOR_ADDERACTOR__H__
#define __PERTURB_INCLUDE_ACTOR_ADDERACTOR__H__
#include "perturb.h"
#include "Actor.h"
#include <deque>
#include <vector>

namespace Perturb 
{

template <typename type>
class AdderActor : public Perturb::Actor
{
  public:

  static const int IDInputA = 0;
  static const int IDInputB = 1;
  static const int IDLockInputA = 2;
  static const int IDLockInputB = 3;
  static const int IDSetLockedValue = 4;
  static const int IDSummedOutput = 5;
  AdderActor(Perturb::Domain& domain) : Actor(domain) 
  {
    this->a = 0;
    this->b = 0;
    AddInputHandler<type>(&Perturb::AdderActor<type>::InputA, IDInputA);
    AddInputHandler<type>(&Perturb::AdderActor<type>::InputB, IDInputB);
    AddInputHandler<bool>(&Perturb::AdderActor<type>::LockInputA, IDLockInputA);
    AddInputHandler<bool>(&Perturb::AdderActor<type>::LockInputB, IDLockInputB);
    AddInputHandler<type>(&Perturb::AdderActor<type>::SetLockedValue, IDSetLockedValue);
    AddOutput<type>(IDSummedOutput);
  }

  private:

  bool LockInputA = false;
  bool LockInputB = false;
  type LockedValue = 0;

  int a, b;
  std::deque<type> bufferA;
  std::deque<type> bufferB;


  void IDLockInputA(bool& value)
  {
    if(value == true)
    {
      this->a = 0;
      this->bufferA.clear();
      this->LockInputA = true;
      this->LockInputB = false;
    } else {
      this->LockInputA = false;
    }
  }
  void IDLockInputB(bool& value)
  {
    if(value == true)
    {
      this->b = 0;
      this->bufferB.clear();
      this->LockInputB = true;
      this->LockInputA = false;
    } else {
      this->LockInputB = false;
    }
  }
  void SetLockedValue(type& value)
  {
    this->LockedValue = value;
  }
  void Reset()
  {
      this->a = 0;
      this->b = 0;
      this->bufferA.clear();
      this->bufferB.clear();
  }
  void InputA(type& A)
  {
    if(this->LockInputA != true)
    {
      this->bufferA.push_back(A);
      this->a++; 
    }
  }
  void InputB(type& B)
  {
    if(this->LockInputB != true)
    {
      this->bufferB.push_back(B);
      this->b++;
    }
  }
  void doWork()
  { 
    if(this->LockInputA == true)
    {
        while(b > 0)
        {
          type c = this->LockedValue + this->bufferB.front();
          this->bufferB.pop_front();
          this->writeOutput<type>(c, 0);
          b--;
        }
    } else if(this->LockInputB == true) {
        while(a > 0)
        {
          type c = this->LockedValue + this->bufferA.front();
          this->bufferA.pop_front();
          this->writeOutput<type>(c, 0);
          a--;
        }
    } else {
      if((a > 0) && (b > 0))
      {
        while((a > 0) && (b > 0))
        {
          type c = this->bufferA.front() + this->bufferB.front();
          this->bufferA.pop_front();
          this->bufferB.pop_front();
          this->writeOutput<type>(c, 0);
          a--; b--;
        }
      }
    }    
  }
};

};

#endif
