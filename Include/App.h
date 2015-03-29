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
#include <Theron/Theron.h> 
#include <perturb.h>
#include <ActorInputMessage.h>
#include <ActorRemoveLink.h>
#include <ActorAddLink.h>


namespace Perturb
{
  class App
  {
		private:
		  Theron::Receiver Receiver;
			Theron::Framework Framework;
		  
		public:
			App()
			{
			}
			Perturb::Address getAddress()
			{
				return this->Receiver.GetAddress();
			}
			Theron::Framework& getFramework()
			{
				return this->Framework;
			}
			template <typename Type>
			bool SendToInput(const Type& Value, int InputID, const Perturb::Address& To)
			{
				return this->SendToInput<Type>(InputID, Value, To, this->Receiver.GetAddress(), 0);
			}
			template <typename Type>
			bool SendToInput(int InputID, const Type& Value, const Perturb::Address& To, int Token)
			{
				return this->SendToInput<Type>(InputID, Value, To, this->Receiver.GetAddress(), 0);
			}
			template <typename Type>
			bool SendToInput(int InputID, const Type& Value, const Perturb::Address& To, const Perturb::Address& From)
			{
				return this->SendToInput<Type>(InputID, Value, To, From, 0);
			}
			template <typename Type>
			bool SendToInput(int InputID, const Type& Value, const Perturb::Address& To, const Perturb::Address& From, int Token)
			{
				Perturb::ActorInputMessage<Type> msg;
				msg.InputID = InputID;
				msg.Token = Token;
				msg.Value = Value;
				return this->Framework.Send(msg, From, To);
			}

			template <typename T>
			bool Link(const Perturb::Address& Source, const Perturb::Address& Destination, int OutputID, int InputID)
			{
				std::type_index Type(typeid(T));
				Perturb::ActorAddLink msg;
				msg.OutputID = OutputID;
				msg.InputID = InputID;
				msg.Address = Destination;
				msg.Type = Type.hash_code();
				return this->Framework.Send(msg, this->Receiver.GetAddress(), Source);
			}
			template <typename T>
			bool UnLink(const Perturb::Address& Source, const Perturb::Address& Destination, int OutputID, int InputID)
			{
				Perturb::ActorRemoveLink msg;
				msg.OutputID = OutputID;
				msg.InputID = InputID;
				msg.Address = Destination;
				msg.Type = std::type_index(typeid(T)).hash_code();
				return this->Framework.Send(msg, this->Receiver.GetAddress(), Source);
			}
	};	
};
#endif
