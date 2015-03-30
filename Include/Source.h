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


#ifndef __PERTURB_INCLUDE_SOURCE_H__
#define __PERTURB_INCLUDE_SOURCE_H__

#include <Theron/Theron.h> 
#include <typeinfo>       
#include <typeindex>      
#include <unordered_map>  
#include <stdexcept>
#include <deque>
#include <algorithm>
#include <perturb.h>
#include <App.h>
#include <ActorAddLink.h>
#include <ActorRemoveLink.h>

namespace Perturb
{

class Source
{
  private:
  int token_ = 0;
  Perturb::App& app_;
  Theron::Receiver receiver_;
  Theron::Catcher<ActorAddLink> add_link_buffer_;
  Theron::Catcher<ActorRemoveLink> remove_link_buffer_;
  Theron::Framework& framework_;
  std::unordered_map<size_t, std::unordered_map<int, std::deque<std::pair<int, Perturb::Address> > > > output_map_;
  /**
   * Registers the catcher push functions with reciever_ 
   */
  void Init()
  {
    this->receiver_.RegisterHandler(&this->add_link_buffer_, &Theron::Catcher<Perturb::ActorAddLink>::Push);
    this->receiver_.RegisterHandler(&this->remove_link_buffer_, &Theron::Catcher<Perturb::ActorRemoveLink>::Push);
  }
  
  /**
   * Handles all add and remove link requests.
   */
  void HandleOutstandingMessages()
  {
    while(this->add_link_buffer_.Empty()!=true)
    {
      ActorAddLink message;
      Address address;
      this->add_link_buffer_.Pop(message, address);
      if(this->output_map_.count(message.Type) != 1)
        continue;
      if(this->output_map_[message.Type].count(message.OutputID) != 1)
        continue;
      std::pair<int, Perturb::Address> pair(message.InputID, message.Address);
      this->output_map_[message.Type][message.OutputID].push_back(pair);
    }
    
    while(this->remove_link_buffer_.Empty()!=true)
    {
      ActorRemoveLink message;
      Address address;
      this->remove_link_buffer_.Pop(message, address);
      if(this->output_map_.count(message.Type) != 1)
        continue;
      if(this->output_map_[message.Type].count(message.OutputID) != 1)
        continue;
      auto it = std::find_if(this->output_map_[message.Type][message.OutputID].begin(),
                             this->output_map_[message.Type][message.OutputID].end(),
		  [&] (const std::pair<int, Perturb::Address> & v) -> bool
		  {
			  if(v.first == message.InputID && v.second == message.Address)
			  	return true;
				
			  return false;        
		  });
		  
		  if((*it).first == message.InputID && (*it).second == message.Address)
		  {
		    this->output_map_[message.Type][message.OutputID].erase(it);
		  }
    }
  }
  public:
  Source(Perturb::App &App) : app_(App), framework_(App.getFramework())
  {
    this->Init();
  }
  Source(Perturb::App &App, int token) : app_(App), framework_(App.getFramework()), token_(token)
  {
    this->Init();
  }
  /**
   * Sends a message from the specified output. The output must have been 
   * created already by a call to CreateOutput. 
   * 
   * @param value The value the message should carry.
   * @param outputid The id of the output to write to.
   * @retun true on message send success. 
   */
  template <typename T>
  bool WriteToOutput(const T& value, int outputid)
  {
    return this->WriteToOutput(value, this->token_, outputid);
  }
  /**
   * Sends a message from the specified output. The output must have been 
   * created already by a call to CreateOutput. 
   * 
   * @param value The value the message should carry.
   * @param outputid The id of the output to write to.
   * @param token The token the message should present.
   * @retun true on message send success. 
   */
  template <typename T>
  bool WriteToOutput(const T& value, int token, int outputid)
  {
    this->HandleOutstandingMessages();
    /*We only need the hash code once per function template.*/
    static std::type_index type = std::type_index(typeid(T));
    static size_t hash = type.hash_code();
    auto present = this->output_map_.count(hash);
    if(present != 1)
    {
      return false;
    }
    present = this->output_map_[hash].count(outputid);
    if(present!=1)
    {
      return false;
    }
    try {
      /*Use at because creation of new elements is undesired*/
      std::deque<std::pair<int, Perturb::Address> >& list = this->output_map_.at(hash).at(outputid);
      Perturb::Address from_address = this->receiver_.GetAddress();
      std::for_each(list.begin(), list.end(), 
        [&] (const std::pair<int, Perturb::Address>& a) -> void 
        {
          Perturb::ActorInputMessage<T> message;
					message.InputID = a.first;
					message.Value = value;
					message.Token = token;
          this->framework_.Send(message, from_address, a.second);
        });
     } catch (const std::out_of_range& oor) {
      /*Output not found so return to caller*/ 
      return false;
     }
     
     return true;
  }
  /**
   * Returns the address of the Source object
   * 
   * @return The address of the source object.
   */
  Perturb::Address GetAddress() const
  {
    return this->receiver_.GetAddress();
  }
  
  template <typename T>
  bool AddOutput(int outputid)
  {
    static size_t hash = std::type_index(typeid(T)).hash_code();
    std::deque<std::pair<int, Perturb::Address> > list;
    auto retpair = this->output_map_[hash].emplace(outputid, list);
    return retpair.second;
  }
  template <typename T>
  bool DeleteOutput(int outputid)
  {
    static size_t hash = std::type_index(typeid(T)).hash_code();
    auto present = this->output_map_.count(hash);
    if(present != 1)
    {
      return false;
    }
    present = this->output_map_[hash].count(outputid);
    if(present == 1)
    {
      this->output_map_[hash].erase(outputid);
      return true;
    }
  }
  
  int set_token(int token)
  {
    this->token_ = token;
    return this->token_;
  }
  int get_token() const
  {
    return this->token_;
  }
};

};
#endif
