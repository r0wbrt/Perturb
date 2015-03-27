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

#ifndef __PERTURB_INCLUDE_APP__H__
#include <Theron/Theron.h> 
#include <Domain.h>
#include <perturb.h>
#include <ActorInputMsg.h>
#define __PERTURB_INCLUDE_APP__H__
namespace Perturb
{
  class App
  {
    private:
      int Token = 0;
      Theron::Framework Framework;
      Theron::Receiver Receiver;
      
      
    
    public:
      App(int Token)
      {
        this->Token = Token;
      }
      App()
      {
        this->Token = 0; 
      }
      Perturb::Domain getDomain()
      {
        Perturb::Domain d(this->Receiver.GetAddress(), this->Token, this->Framework);
        return d;
      }
      Theron::Framework& getFramework()
      {
        return this->Framework;
      }
      template <typename Type>
      bool SendToInput(int InputID, Type& Value, Perturb::Address To)
      {
        return this->SendToInput<Type>(InputID, Value, To, this->Receiver.GetAddress(), this->Token);
      }
      template <typename Type>
      bool SendToInput(int InputID, Type& Value, Perturb::Address To, int Token)
      {
        return this->SendToInput<Type>(InputID, Value, To, this->Receiver.GetAddress(), Token);
      }
      template <typename Type>
      bool SendToInput(int InputID, Type& Value, Perturb::Address To, Perturb::Address From)
      {
        return this->SendToInput<Type>(InputID, Value, To, From, this->Token);
      }
      template <typename Type>
      bool SendToInput(int InputID, Type& Value, Perturb::Address To, Perturb::Address From, int Token)
      {
        Perturb::ActorInputMsg<Type> msg(InputID, Value, Token);
        return this->Framework.Send(msg, From, To);
      }
  };


};
#endif
