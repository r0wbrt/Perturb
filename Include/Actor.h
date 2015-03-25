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


#ifndef __PERTURB_INCLUDE_ACTOR__H__
#define __PERTURB_INCLUDE_ACTOR__H__

#include <Theron/Theron.h> 
#include <typeinfo>       
#include <typeindex>      
#include <unordered_map>  
#include <stdexcept>

typedef Theron::Address Perturb::Actor::Address

  class Perturb::Actor : public Theron::Actor 
  {
    public:
    
    /*Created to prevent potential message type conflicts*/
    class InputMsg
    {
      public:
        int InputID;
        T Value;
        InputMsg(int InputID, T Value) : InputID(InputID), Value(Value)
        {}
    };

    Actor(Theron::Framework &framework) : Theron::Actor(framework)
    {
      RegisterHandler(this, &perturb::actor::handleAddInputLink);
      RegisterHandler(this, &perturb::actor::handleEvent<int>);
    }
    protected:
    virtual void doWork() = 0;
    Perturb::Actor::Address getFromAddress()
    {
      return this->WhoFrom;
    }
    /**
      * 
      */
    template <typename T>
    bool AddInputHandler(void (*callback)(const T&), int InputID)
    {
      /*Insert Function into correct map by type*/
      std::unordered_map<int, void *>& map = this->InputMap[std::typeid(T)];
      map[InputID] = (void *)callback;  

      /*Register Handler for this type if not already done*/
      if(IsHandlerRegistered(this, &Perturb::Actor::InputMsgHandler<T>) != true)
        return RegisterHandler(this, &Perturb::Actor::InputMsgHandler<T>);

      return true;
    }

    template <typename T>
    bool RemoveInputHandler(int InputID)
    {
      try {
        std::unordered_map<int, void *>& map = this->InputMap.at(std::typeid(T));
        map.erase(InputID); 
        return true;
      } catch (const std::out_of_range& oor) {return false; }
    }

    template <typename T>
    void InputMsgHandler(const Perturb::Actor::InputMsg& msg,  const Theron::Address from)
    {
      try {
        std::unordered_map<int, void *>& map = this->InputMap.at(std::typeid(T));
        void (*f)(T&) = map.at(msg.InputID);
        this->WhoFrom = from;
        *this.*f(msg.Value);
      } catch (const std::out_of_range& oor) {}
        this->doWork();
    }

    template <typename T>
    bool AddOutput(int OutputID);
    template <typename T>    
    bool RemoveOutput(int OutputID);
    template <typename T>
    bool WriteOutput(int OutputID, T& value);

    template <typename T>
    bool AddField(std::string Name, void (*setter)(const T&), T (*getter)());
    template <typename T>    
    bool RemoveField(std::string Name);

    template <typename T>
    T setField(std::string Name, T& Value, Perturb::Actor::Address Address);
    template <typename T>
    void SetFieldMsgHandler(const Perturb::Actor::SetField<T>& msg, const Theron::Address from);

    template <typename T>
    T getField(std::string Name, Perturb::Actor::Address Address, void (*callback)(const T&, void * p), void * p);
    template <typename T>
    void GetFieldQueryMsgHandler(const Perturb::Actor::GetFieldQuery<T>& msg, const Theron::Address from);
    template <typename T>
    void GetFieldReplyMsgHandler(const Perturb::Actor::GetFieldReply<T>& msg, const Theron::Address from);


    template <typename T>
    void AddLinkMsgHandler(const Perturb::Actor::AddLink<T>& msg, const Theron::Address from);
    template <typename T>
    void RemoveLinkMsgHandler(const Perturb::Actor::RemoveLink<T>& msg, const Theron::Address from);
    

  private:
    Perturb::Actor::Address WhoFrom;
    std::unordered_map<std::type_index, std::unordered_map<int, void *> > InputMap;
};


#endif
