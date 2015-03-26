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

namespace Perturb 
{



  class Actor : public Theron::Actor 
  {
    public:
    typedef Theron::Address Address;
    
    /*Created to prevent potential message type conflicts*/
    template <typename T>
    class InputMsg
    {
      public:
        int InputID;
        int Token;
        T Value;
        InputMsg(int InputID, T Value) : InputID(InputID), Value(Value), Token(0)
        {}
        InputMsg(int InputID, T Value, int Token) : InputID(InputID), Value(Value), Token(Token)
        {}
    };

    class SyncRequestMessage
    {
      public:
        bool ResetState = false;
        bool ChangeToken = false;
        bool ChangeDomainCheck = false;
        bool DomainCheck = false;
        bool ChangeDomainMasterAddress = false;
        Perturb::Actor::Address NewDomainMasterAddress;
        int Token = 0;
        int ID;
        SyncRequestMessage(bool ResetState, int ID) : ResetState(ResetState), ID(ID)
        {}
        SyncRequestMessage(int Token, bool ResetState) : ResetState(ResetState), ChangeToken(true), Token(Token), ID(ID)
        {}
        SyncRequestMessage(int ID) : ID(ID)
        {}
    };

    class SyncReplyMessage
    {
      public:
        int Token;
        int ID;
      SyncReplyMessage(int Token, int ID) : Token(Token), ID(ID)
      {}
    };
    
    template <typename T>    
    class AddLink
    {
      public:
        int OutputId;
        int InputId;
        Perturb::Actor::Address Address;
        AddLink(int OutputId, int InputId, Perturb::Actor::Address Address) : OutputId(OutputId), InputId(InputId), Address(Address)
        {}
    };


    template <typename T>    
    class RemoveLink
    {
      public:
        int OutputId;
        int InputId;
        Perturb::Actor::Address Address;
        AddLink(int OutputId, int InputId, Perturb::Actor::Address Address) : OutputId(OutputId), InputId(InputId), Address(Address)
        {}
    };


    Actor(Perturb::Domain& domain) : Theron::Actor(domain.getFrameWork())
    {
      this->DomainMasterAddress = domain.getAddress();  
      this->DomainToken = domain.getToken();
      this->DomainMasterCheck = domain.isDomainMasterCheck();    
    }
    protected:
    virtual void doWork() {};
    virtual void reset() {};
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
    void InputMsgHandler(const Perturb::Actor::InputMsg<T>& msg,  const Theron::Address from)
    {
      if(msg.Token != this->Token)
        return void;
      try {
        std::unordered_map<int, void *>& map = this->InputMap.at(typeid(T));
        void (*f)(T&) = map.at(msg.InputID);
        this->WhoFrom = from;
        *this.*f(msg.Value);
      } catch (const std::out_of_range& oor) {}
        this->doWork();
    }

    template <typename T>
    bool AddOutput(int OutputID)
    {
      std::unordered_map<int, std::deque<std::pair<int, Perturb::Actor::Address> > >& map = this->OutputMap[typeid(T)];
      std::deque<std::pair<int, Perturb::Actor::Address> > que;
      map[OutputId] = que;

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
        std::unordered_map<int, std::deque<std::pair<int, Perturb::Actor::Address> > >& map = this->OutputMap.at(typeid(T));
        map.erase(OutputID);
      } catch (const std::out_of_range& oor) {}

      return true;
    }
    template <typename T>
    bool WriteOutput(int OutputID, T& value)
    {
      try {
        std::unordered_map<int, std::deque<std::pair<int, Perturb::Actor::Address> > >& map = this->OutputMap.at(typeid(T));
        std::deque<std::pair<int, Perturb::Actor::Address> >& list = map.at(OutputId);
        std::for_each(list.begin(), list.end(), [&] (const T& a) -> void 
          {
            Perturb::Actor::InputMsg<T> msg(a.first, value, this->token);
            this->Send(msg, a.second);
          });
      } catch (const std::out_of_range& oor) {return false;}
      return true;   
    }

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
    
    void SyncMsgHandler(const Perturb::Actor::SyncRequestMessage& msg, const Theron::Address from)
    {
      if(this->DomainMasterCheck==true)
      {
        if(this->DomainMasterAddress != from)
          return void;
      }
      
      if(msg.ChangeDomainCheck == true)
        this->DomainMasterCheck = msg.DomainCheck;

      if(msg.ChangeDomainMasterAddress == true)
        this->DomainMasterAddress = msg.NewDomainMasterAddress;
      
      if(msg.ChangeToken == true)
        this->Token = msg.Token;
  
      if(msg.ResetState == true)
        this->Reset();

      this->Send(Perturb::Actor::SyncReplyMessage(msg.Token, msg.ID), from);
      
      this->doWork();
    }
      

  private:
    Perturb::Actor::Address WhoFrom;
    std::unordered_map<std::type_index, std::unordered_map<int, void *> > InputMap;
    std::unordered_map<std::type_index, std::unordered_map<int, std::deque<std::pair<int, Perturb::Actor::Address> > > > OutputMap;
    int Token = 0;
    Perturb::Actor::Address DomainMasterAddress;
    bool DomainMasterCheck = false;
};


};
#endif
