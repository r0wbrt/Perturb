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
#define __PERTURB_INCLUDE_APP__H__
namespace Perturb
{
  class App
  {
    private:
      int Token;
      Theron::Framework& Framework;
      Theron::Receiver Receiver;
      
      
    
    public:
      Perturb::Actor::Domain getDomain()
      {
        Perturb::Actor::Domain d(this->Receiver.GetAddress(), this->Token, this->Framework);
        return d;
      }
      Theron::Framework& getFramework()
      {
        return this->Framework;
      }
      template <typename Type>
      bool SendToInput(int InputID, T& Value, Actor::Address To);
      template <typename Type>
      bool SendToInput(int InputID, T& Value, Actor::Address To, int Token);
      template <typename Type>
      bool SendToInput(int InputID, T& Value, Actor::Address To, Actor::Address From);
      template <typename Type>
      bool SendToInput(int InputID, T& Value, Actor::Address To, Actor::Address From, int Token);
  };

  template <typename Type>
  bool MakeLink(Actor& fromActor, Actor& toActor, int OutputID, int InputID);
  temp  late <typename Type>
  bool MakeLink(App& fromActor, Actor& toActor, int OutputID, int InputID);
  template <typename Type>
  bool MakeLink(Actor fromActor, App& toActor, int OutputID, int InputID);
  template <typename Type>
  bool MakeLink(App& fromActor, App& toActor, int OutputID, int InputID);
  
  template <typename Type>
  bool BreakLink(Actor& fromActor, Actor& toActor, int OutputID, int InputID);
  template <typename Type>
  bool BreakLink(App& fromActor, Actor& toActor, int OutputID, int InputID);
  template <typename Type>
  bool BreakLink(Actor fromActor, App& toActor, int OutputID, int InputID);
  template <typename Type>
  bool BreakLink(App& fromActor, App& toActor, int OutputID, int InputID);


};
#endif
