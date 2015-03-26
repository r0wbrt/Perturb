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
  AdderActor(Perturb::Domain& domain) : Actor(domain) 
  {
    this->a = 0;
    this->b = 0;
    AddInputHandler<type>(&Perturb::AdderActor<type>::InputA, 0);
    AddInputHandler<type>(&Perturb::AdderActor<type>::InputB, 1);
    AddOutput<type>(0);
  }

  private:

  int a, b;
  std::deque<type> bufferA;
  std::deque<type> bufferB;

  void Reset()
  {
      this->a = 0;
      this->b = 0;
      this->bufferA.clear();
      this->bufferB.clear();
  }
  void InputA(type& A)
  {
    if(this->pause == true)
      return void;
    this->bufferA.push_back(A);
    this->a++; 
  }
  void InputB(type& B)
  {
    if(this->pause == true)
      return void;

    this->bufferB.push_back(B);
    this->b++;
  }
  void doWork()
  { 
    if(this->pause == true)
      return void;

    if((a > 0) && (b > 0))
    {
      while((a > 0) && (b > 0))
      {
        type c = this->bufferA.front() + this->bufferB.front();
        this->bufferA.pop_front();
        this->bufferB.pop_front();
        this->writeOutput<double>(c, 0);
        a--; b--;
      }
    }    
  }
};

};

#endif
