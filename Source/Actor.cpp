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

void Actor::SyncMsgHandler(const Perturb::ActorSyncRequestMessage& msg, const Theron::Address from)
{
	if(msg.ChangeToken == true)
	{
		this->Token = msg.Token; 
		this->TokenChanged(msg.Token);
	}
	
	if(msg.ResetState == true)
		this->Reset();
		
	this->Send(Perturb::ActorSyncReplyMessage{msg.Token, msg.ID}, from);
	this->doWork();
}
void Actor::AddLinkMsgHandler(const Perturb::ActorAddLink& msg, const Theron::Address from)
{
	try 
	{
		std::unordered_map<int, std::deque<std::pair<int, Perturb::Address> > >& map = this->OutputMap.at(msg.Type);
		std::deque<std::pair<int, Perturb::Address> >& list = map.at(msg.OutputID);
		list.push_back(std::pair<int, Perturb::Address>(msg.InputID, msg.Address));
	} 
	catch (const std::out_of_range& oor)
	{/*Do Nothing*/}
}

void Actor::RemoveLinkMsgHandler(const Perturb::ActorRemoveLink& msg, const Theron::Address from)
{
	try
	 {
		std::unordered_map<int, std::deque<std::pair<int, Perturb::Address> > >& map = this->OutputMap.at(msg.Type);
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

Actor::Actor(Perturb::App& App) : App(App), Theron::Actor(App.getFramework())
{
	RegisterHandler(this, &Perturb::Actor::SyncMsgHandler);
	RegisterHandler(this, &Perturb::Actor::AddLinkMsgHandler);
	RegisterHandler(this, &Perturb::Actor::RemoveLinkMsgHandler);
}

bool Actor::ToggleTokenCheck() 
{
	this->TokenCheck = !this->TokenCheck;
	return this->TokenCheck;
}
int Actor::getMessageToken()
{
	return this->MessageToken;
}
int Actor::getMessageInputID()
{
	return this->MessageInputID;
}
Perturb::Address& Actor::getMessageFromAddress()
{
	return this->MessageFrom;
}

};
