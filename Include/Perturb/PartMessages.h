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

#ifndef __PERTURB_INCLUDE_PERTURB_PARTMESSAGES_H_
#define __PERTURB_INCLUDE_PERTURB_PARTMESSAGES_H_
#include <Perturb/Perturb.h>
#include <deque>
namespace Perturb {

struct PartInterfaceLinkMessage
{
  NameHash input_hash;
  NameHash output_hash;
  Perturb::Address address;
  TypeHash type;
  bool add;
};

template <typename T>
struct PartInterfaceInputMessage
{
  TypeHash type;
  NameHash input_hash;
  NameHash output_hash;
  T payload;
  Perturb::ContextToken token;
};
struct PartInterfaceGetListMessage
{
  bool reply;
  bool input_list;
  Perturb::Address reply_address;
  std::deque<std::pair<std::string, TypeHash> > list;
  int id;
};
struct PartInterfaceSyncMessage
{
  bool reply;
  bool reset;
  bool update_token;
  int id;
  ContextToken token;
};


};

#endif
