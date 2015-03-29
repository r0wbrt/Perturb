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

#include <Actor.h>
#include <iostream>
namespace Perturb {

void Actor::SyncMessageHandler(const Perturb::ActorSyncRequestMessage& msg, const Theron::Address from)
{
	if(msg.ChangeToken == true)
	{
		this->token_ = msg.Token; 
		this->TokenChanged(msg.Token);
	}
	
	if(msg.ResetState == true)
		this->Reset();
		
	this->Send(Perturb::ActorSyncReplyMessage{msg.Token, msg.ID}, from);
	this->doWork();
}
void Actor::AddLinkMessageHandler(const Perturb::ActorAddLink& msg, const Theron::Address from)
{
	try 
	{
		std::unordered_map<int, std::deque<std::pair<int, Perturb::Address> > >& map = this->output_map_.at(msg.Type);
		std::deque<std::pair<int, Perturb::Address> >& list = map.at(msg.OutputID);
		list.push_back(std::pair<int, Perturb::Address>(msg.InputID, msg.Address));
	} 
	catch (const std::out_of_range& oor)
	{/*Do Nothing*/}
}

void Actor::RemoveLinkMessageHandler(const Perturb::ActorRemoveLink& msg, const Theron::Address from)
{
	try
	 {
		std::unordered_map<int, std::deque<std::pair<int, Perturb::Address> > >& map = this->output_map_.at(msg.Type);
		std::deque<std::pair<int, Perturb::Address> >& list = map.at(msg.OutputID);
		auto it = std::find_if(list.begin(), list.end(), 
		[&] (const std::pair<int, Perturb::Address> & v) -> bool
		{
			if(v.first == msg.InputID && v.second == msg.Address)
				return true;
				
			return false;        
		}
		);

	if((*it).first == msg.InputID && (*it).second == msg.Address)
		list.erase(it);
	} 
	catch (const std::out_of_range& oor) 
	{/*Do nothing*/}
}

Actor::Actor(Perturb::App& App) : app_(App), Theron::Actor(App.getFramework())
{
	RegisterHandler(this, &Perturb::Actor::SyncMessageHandler);
	RegisterHandler(this, &Perturb::Actor::AddLinkMessageHandler);
	RegisterHandler(this, &Perturb::Actor::RemoveLinkMessageHandler);
}

bool Actor::ToggleTokenCheck() 
{
	this->token_check_ = !this->token_check_;
	return this->token_check_;
}
int Actor::get_message_token()
{
	return this->message_token_;
}
int Actor::get_message_input_id()
{
	return this->message_input_id_;
}
Perturb::Address& Actor::get_message_from_address()
{
	return this->message_from_address_;
}

};
