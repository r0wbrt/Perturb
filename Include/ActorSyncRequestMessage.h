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


#ifndef __PERTURB_INCLUDE_ActorSyncRequestMessage__H__
#define __PERTURB_INCLUDE_ActorSyncRequestMessage__H__

#include <Theron/Theron.h> 
#include <perturb.h>
namespace Perturb
{
   class ActorSyncRequestMessage
    {
      public:
        bool ResetState = false;
        bool ChangeToken = false;
        bool ChangeDomainMasterAddress = false;
        Perturb::Address NewDomainMasterAddress;
        int Token = 0;
        int ID;
        ActorSyncRequestMessage(bool ResetState, int ID) : ResetState(ResetState), ID(ID)
        {}
        ActorSyncRequestMessage(int Token, bool ResetState) : ResetState(ResetState), ChangeToken(true), Token(Token), ID(ID)
        {}
        ActorSyncRequestMessage(int ID) : ID(ID)
        {}
    };
};
#endif
   
