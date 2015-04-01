#include <Perturb/Perturb.h>
#include <Perturb/Part.h>

namespace Perturb {

class Interface : public Part
{
  public:
  bool AddOutput(const std::string& name, std::type_index type);
  bool RemoveOutput(const std::string& name, std::type_index type);
  template <typename T>
  bool WriteToOutput(const T& value, const std::string& name);
  template <typename T>
  bool WriteToOutput(const T& value, size_t hash);
  
  bool IsInputEmpty(const std::string& name, std::type_index type);
  bool IsInputEmpty(const std::string& name, size_t type_hash);
  bool IsInputEmpty(size_t name_hash, size_t type_hash);
  
  template <typename T>
  bool AddInput(const std::string& name);
  template <typename T>
  bool RemoveInput(const std::string& name);
  
  void Wait();
  void Wait(int number);
  
  template <typename T>
  bool ReadInput(const std::string& name, T& value, Perturb::Address& from)
  template <typename T>
  bool ReadInput(size_t name_hash, T& value, Perturb::Address& from)
};

};
