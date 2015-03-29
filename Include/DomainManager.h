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


#ifndef __PERTURB_INCLUDE_DomainManager__H__
#define __PERTURB_INCLUDE_DomainManager__H__

#include <deque>
#include <unordered_map>
#include <atomic>
#include <algorithm>
#include <perturb.h>
#include <App.h>
#include <Theron/Theron.h> 
#include <ActorSyncRequestMessage.h>
#include <ActorSyncReplyMessage.h>

namespace Perturb {

class DomainManager
{
	private:
		int Token = 0;
		Theron::Framework& Framework;
		Theron::Receiver Receiver;
		std::deque<Perturb::Address> List;
		std::unordered_map<int, Perturb::Address> OutstandingSyncMessages;
		Theron::Catcher<Perturb::ActorSyncReplyMessage> SyncReplyBuffer;
		Perturb::App& App;
		int syncCounter = 1;
		int genNewSyncID();
		void IssueResetMessage(Perturb::Address Address);
		void IssueNewTokenMessage(Perturb::Address Address, int Token);


  public:
		DomainManager(Perturb::App& App);
		DomainManager(Perturb::App& App, int Token);
		bool AddToDomain(Perturb::Address ActorAddress);
		bool RemoveFromDomain(Perturb::Address ActorAddress);    
		bool waitForSync();
		bool isInSync();
		bool ResetDomain();
		static int genToken();
		int setToken(int Token);
		int getToken();
};

};
#endif
