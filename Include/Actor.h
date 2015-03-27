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
#include <deque>
#include <algorithm>
#include <App.h>
#include <perturb.h>
#include <ActorAddLink.h>
#include <ActorInputMsg.h>
#include <ActorRemoveLink.h>
#include <ActorSyncReplyMessage.h>
#include <ActorSyncRequestMessage.h>
#include <Domain.h>

namespace Perturb 
{
  class Actor : public Theron::Actor 
  {
    public:

    Actor(Perturb::App& App) : App(App), Theron::Actor(App.getFramework())
    {
      RegisterHandler(this, &Perturb::Actor::SyncMsgHandler);
      this->DomainMasterAddress = App.getAddress();  
      this->Token = App.getToken();  
    }
    protected:
    bool toggleTokenCheck() 
    {
      this->TokenCheck = !this->TokenCheck;
      return this->TokenCheck;
    }
    int getMessageToken()
    {
      return this->MessageToken;
    }
    virtual void doWork() {};
    virtual void Reset() {};
    Perturb::Address getFromAddress()
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
      std::unordered_map<int, void *>& map = this->InputMap[typeid(T)];
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
        std::unordered_map<int, void *>& map = this->InputMap.at(typeid(T));
        map.erase(InputID); 
        return true;
      } catch (const std::out_of_range& oor) {return false; }
    }

    template <typename T>
    void InputMsgHandler(const Perturb::ActorInputMsg<T>& msg,  const Theron::Address from)
    {
      if(msg.Token != this->Token && this->TokenCheck == true)
        return ;
      try {
        std::unordered_map<int, void *>& map = this->InputMap.at(typeid(T));
        void (*f)(T&) = map.at(msg.InputID);
        this->WhoFrom = from;
        this->MessageToken = msg.Token;
        *this.*f(msg.Value);
      } catch (const std::out_of_range& oor) {}
        this->doWork();
    }

    template <typename T>
    bool AddOutput(int OutputID)
    {
      std::unordered_map<int, std::deque<std::pair<int, Perturb::Address> > >& map = this->OutputMap[typeid(T)];
      std::deque<std::pair<int, Perturb::Address> > que;
      map[OutputID] = que;

      if(IsHandlerRegistered(this, &Perturb::Actor::AddLinkMsgHandler<T>) != true)
        RegisterHandler(this, &Perturb::Actor::AddLinkMsgHandler<T>);
  
      if(IsHandlerRegistered(this, &Perturb::Actor::RemoveLinkMsgHandler<T>) != true)
        RegisterHandler(this, &Perturb::Actor::RemoveLinkMsgHandler<T>);

      return true;
    }
    template <typename T>    
    bool RemoveOutput(int OutputID)
    {
      try {
        std::unordered_map<int, std::deque<std::pair<int, Perturb::Address> > >& map = this->OutputMap.at(typeid(T));
        map.erase(OutputID);
      } catch (const std::out_of_range& oor) {}

      return true;
    }
    template <typename T>
    bool WriteOutput(int OutputID, const T& value)
    {
      return this->WriteOutput<T>(OutputID, this->Token, value);
    }
    template <typename T>
    bool WriteOutput(int OutputID, int Token, const T& value)
    {
      try {
        std::unordered_map<int, std::deque<std::pair<int, Perturb::Address> > >& map = this->OutputMap.at(typeid(T));
        std::deque<std::pair<int, Perturb::Address> >& list = map.at(OutputID);
        std::for_each(list.begin(), list.end(), [&] (const T& a) -> void 
          {
            Perturb::ActorInputMsg<T> msg(a.first, value, Token);
            this->Send(msg, a.second);
          });
      } catch (const std::out_of_range& oor) {return false;}
      return true;   
    }

    template <typename T>
    void AddLinkMsgHandler(const Perturb::ActorAddLink<T>& msg, const Theron::Address from)
    {
      try {
        std::unordered_map<int, std::deque<std::pair<int, Perturb::Address> > >& map = this->OutputMap.at(typeid(T));
        std::deque<std::pair<int, Perturb::Address> >& list = map.at(msg.OutputId);
        list.push_back(std::pair<int, Perturb::Address>(msg.InputID, msg.Address));
        } catch (const std::out_of_range& oor) {}
    }
    template <typename T>
    void RemoveLinkMsgHandler(const Perturb::ActorRemoveLink<T>& msg, const Theron::Address from)
    {
      try {
        std::unordered_map<int, std::deque<std::pair<int, Perturb::Address> > >& map = this->OutputMap.at(typeid(T));
        std::deque<std::pair<int, Perturb::Address> >& list = map.at(msg.OutputId);
        auto it = std::find_if(list.begin(), list.end(), [&] (const std::pair<int, Perturb::Address> & v) -> bool
        {
          if(v.first == msg.InputID && v.second == msg.Address)
            return true;
          return false;        
        });

        if(*it.first == msg.InputID && *it.second == msg.Address)
          list.erase(it);

        } catch (const std::out_of_range& oor) {}
    }
    virtual void TokenChanged(int Token) {};
    void SyncMsgHandler(const Perturb::ActorSyncRequestMessage& msg, const Theron::Address from);
      

  private:
    Perturb::Address WhoFrom;
    Perturb::App& App;
    std::unordered_map<std::type_index, std::unordered_map<int, void *> > InputMap;
    std::unordered_map<std::type_index, std::unordered_map<int, std::deque<std::pair<int, Perturb::Address> > > > OutputMap;
    int Token = 0;
    int MessageToken = 0;
    bool TokenCheck = true;
    Perturb::Address DomainMasterAddress;
};


};
#endif
