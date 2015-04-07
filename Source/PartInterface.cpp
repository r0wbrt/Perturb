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
#include <Perturb/PartInterface.h>
#include <functional>

namespace Perturb
{

PartInterface::PartInterface(Theron::Framework& framework) : Actor(framework)
{}

void PartInterface::Log(const std::string& out_message)
{
  static NameHash name = this->HashName("Application::stdout");
  this->SendToInput<std::string>(out_message, name, 0, 0, this->log_output_address_);
}
void PartInterface::LogError(const std::string& out_message)
{
  static NameHash name = this->HashName("Application::stderror");
  this->SendToInput(out_message, name, 0, 0, this->log_output_address_);
}

bool PartInterface::Initialize(Part * part, const Perturb::Address log_address)
{
  this->part_ = part;
  this->log_output_address_ = log_address;
  
  /*Check to make sure every handler is registered, class is useless otherwise*/  
  if(this->RegisterHandler(this, &Perturb::PartInterface::LinkMessageHandler) != true)
    return false;
    
  if(this->RegisterHandler(this, &Perturb::PartInterface::SyncMessageHandler) != true)
    return false;
    
  if(this->RegisterHandler(this, &Perturb::PartInterface::GetListMessageHandler) != true)
    return false;
}

NameHash PartInterface::HashName(std::string name)
{
  std::hash<std::string> hash_fn;
  return hash_fn(name);
}


void PartInterface::GetListMessageHandler(const PartInterfaceGetListMessage& message, const Perturb::Address from)
{

  
  if(message.reply == true)
  {
    this->part_->__internal_set_message_address(from);
    this->part_->ListReply(message.list, message.id);
    return ;
  }
  
  PartInterfaceGetListMessage reply;
  reply.id = message.id;
  reply.reply = true;

  if(message.input_list == true)
  { 
    reply.list = this->input_name_list_;
  } else {
    reply.list = this->output_name_list_;
  }
  
  if(this->Send(reply, message.reply_address) != true)
    this->LogError("Failed to return IO List to sender.");
}
void PartInterface::LinkMessageHandler(const PartInterfaceLinkMessage& message, const Perturb::Address from)
{
  if(this->output_map_.count(message.type)!=1)
    return ;
  if(this->output_map_[message.type].count(message.output_hash)!=1)
    return ;
  
  if(message.add == true)
  {
    ToEntry entry(message.input_hash, message.address);
    this->output_map_[message.type][message.output_hash].push_back(entry);
  } else {
    ToList& list = this->output_map_[message.type][message.output_hash];
    auto it = std::find_if(list.begin(), list.end(), 
    [&message](const ToEntry value) -> bool 
    {
      if((value.first == message.input_hash) && (value.second == message.address))
        return true;
      else
        return false; 
    });
    
    if(((*it).first == message.input_hash) && ((*it).second == message.address))
    {
      list.erase(it);
    }
  }  
}
void PartInterface::SyncMessageHandler(const PartInterfaceSyncMessage& message, const Perturb::Address from)
{
  
  this->part_->__internal_set_message_address(from);
  
  if(message.reply==true)
  {
    this->part_->SyncReply(message.id);
  } else {
    
    if(message.reset == true)
      this->part_->Reset();
      
    if(message.update_token == true)
    {
      this->part_->__internal_set_check_token(message.token);
      this->part_->TokenChanged();
    }
    
    /*Issue a reply to sync sender*/
    PartInterfaceSyncMessage reply_message;
    reply_message.reply = true;
    reply_message.id = message.id;
    if(this->Send(reply_message, from)!=true)
      this->LogError("Failed to issue sync reply to sender. Could render system into an undefined state.");
  }
  
}

int PartInterface::IssueReset(Perturb::Address address)
{
  PartInterfaceSyncMessage message;
  message.reply = false;
  message.update_token = false;
  message.reset = true;
  int id = this->sync_id_++;
  message.id = id;    
  if(this->Send(message, address) != true)
    return -1;
    
  return id;
}

int PartInterface::IssueNewToken(int Token, Perturb::Address address)
{
  PartInterfaceSyncMessage message;
  message.reply = false;
  message.update_token = true;
  message.token = Token;
  message.reset = false;
  int id = this->sync_id_++;
  message.id = id;    
  if(this->Send(message, address) != true)
    return -1;
    
  return id;
}


};
