namespace Perturb {

struct PartInterfaceMakeLinkMessage;
struct PartInterfaceRemoveLinkMessage;
struct PartInterfaceGetFieldMessage;
template <typename T>
struct PartInterfaceSetFieldMessage;
template <typename T>
struct PartInterfaceInputMessage
{
  TypeHash type;
  NameHash input_hash;
  NameHash output_hash;
  T payload;
  Perturb::ContextToken token;
};
struct PartInterfaceGetListMessage;

};

struct PartInterfaceSyncMessage;
