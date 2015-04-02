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
