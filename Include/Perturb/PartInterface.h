#include <Perturb/Perturb.h>
#include <Perturb/Part.h>
#include <Perturb/PartMessages.h>

namespace Perturb {

class Part;

class PartInterface : public Theron::Actor
{
  private:
  typedef std::function<const void(void *)> FunctionPointer;
  typedef size_t TypeHash;
  typedef size_t NameHash;
  typedef std::pair<NameHash, Perturb::Address> ToEntry;
  typedef std::deque<ToEntry> ToList;
  typedef std::unordered_map<NameHash, FunctionPointer> InputMap;
  typedef std::unordered_map<NameHash, ToList> OutputMap;
  
  Part * part_;
  std::unordered_map<TypeHash, InputMap> input_map_;
  std::unordered_map<TypeHash, OutputMap> output_map_;
  std::deque<std::pair<std::string, TypeHash> > input_name_list_;
  std::deque<std::pair<std::string, TypeHash> > output_name_list_;
  Perturb::Address log_output_address_;  
  public:
  PartInterface(Theron::Framework & framework);
  /**
   * Registers the Input Handler of a part with the Part Interface class.
   *
   * @param owner A pointer to the owning class of the function. 
   * @param callback A pointer to the function handling the input.
   * @param name A unique name identifying this input.
   * @return true on success. A false implies the PartInterface is in a 
   *   indeterminate state and should be destroyed.
   */
  template <typename T, typename C>
  bool AddInputHandler(C * owner, void (C::*callback)(const T& Value), const std::string& name)
  {
    static TypeHash type_hash = typeid(T).hash_code();
    
    /*Register Handler for this type if not already done*/
    if(IsHandlerRegistered(this, &Perturb::PartInterface::InputMessageHandler<T>) != true)
    {
      if(RegisterHandler(this, &Perturb::PartInterface::InputMessageHandler<T>) != true)
      {
        this->Log("Register input failed in AddInputHandler.");
        return false;
      }
    }
    std::pair<std::string, TypeHash> name_pair(name, type_hash);
    this->input_name_list_.push_back(name_pair);
    /*Wrap the call back in a lambda function so the callback can be stored
      as a generic function with predictable type signature.*/
    this->input_map_[type_hash].[this->HashName(name)] = [callback, owner] (const void * p)
    -> void
    {
      owner->callback((T*)p);
    };
    
  }
  template <typename T>
  bool RemoveInputHandler(const std::string& name)
  {
    static TypeHash type_hash = typeid(T).hash_code();
    if(this->input_map_.count(type_hash)!=1)
      return false;
    
    if(this->input_map_[type_hash].count(this->HashName(name))!=1)
      return false;
      
    auto it = std::find_if(this->input_name_list_.begin(), this->input_name_list_.end(), 
    [name, type_hash] (const std::pair<std::string, TypeHash> value) -> bool 
    {
      if((value.first == name) && (value.second == type_hash))
      {
        return true;
      } else {
        return false;
      }
    });
      
    if(((*it).first == name) && ((*it).second == type_hash))
      this->input_name_list_.erase(it);
      
    this->input_map_[type_hash].erase(this->HashName(name));
    /*If no more inputs are registered with this type we might as well
      remove the message handler and associated list to free memory*/
    if(this->input_map_[type_hash].empty()== true)
    {
      /*Only remove input map if we can deregister our handler*/
      if(DeregisterHandler(this, &Perturb::PartInterface::InputMessageHandler<T>)==true)
      {
        this->input_map_.erase(type_hash);
      }
    }
  }
  /**
   * Adds a new output. Calling this function tells the PartInterface to allow
   * other parts to link to an output with this name of this type.
   *
   * @param name Output name used by other parts to link to this class.
   * @return true on success.
   */
  template <typename T>
  bool AddOutput(const std::string& name)
  {
    static TypeHash type_hash = typeid(T).hash_code();
    ToList list;
    this->output_map_[type_hash].[this->HashName(name)] = list;
    std::pair<std::string, TypeHash> name_pair(this->HashName(name), type_hash);
    this->ouput_name_list_.push_back(name_pair);
    return true;
  }
  /**
   * Removes an output and deletes all associated links made to the output by
   * other parts. The other parts are not notified of their links being severed.
   *
   * @param name The name of the output to be removed.
   * @return returns true on success.
   */
  template <typename T>
  bool RemoveOutput(const std::string& name)
  {
    static TypeHash type_hash = typeid(T).hash_code();
    /*Verify we actually have a type_hash key*/
    if(this->output_map_.count(type_hash) != 1)
      return false;
      
    /*Verify we actually have a output named 'name'*/
    if(this->output_map_[type_hash].count(this->HashName(name)) != 1)
      return false;
      
    /*Find and remove from ouput_name_list_*/  
    auto it = std::find_if(this->output_name_list_.begin(), this->output_name_list_.end(),
    [name, type_hash] (const std::pair<std::string, TypeHash> value) -> bool 
    {
      if((value.first == name) && (value.second == type_hash))
      {
        /*find if will stop when we return true*/
        return true;
      } else {
        return false;
      }
    });
    
    /*Check to make sure we have the right one*/
    if(((*it).first == name) && ((*it).second == type_hash))
      this->output_name_list_.erase(it);
     
    /*If no more list exist under this type remove this type key from the
      map*/
    this->output_map_[type_hash].erase(this->HashName(name));
    if(this->output_map[type_hash].empty() == true)
    {
      this->output_map.erase(type_hash);
    }
    
    return true;
  }
  
  template <typename T>
  bool WriteToOutput(const T& value, const std::string& name);
  template <typename T>
  bool WriteToOutput(const T& value, NameHash hash);


  template <typename T, typename C>
  void AddGetter(C * owner, T (C::*callback)(), const std::string& name);
  template <typename T, typename C>
  void AddSetter(C * owner, void (C::*callback)(const T& value), const std::string& name);
  
  template <typename T>
  void RemoveGetter(const std::string& name);
  template <typename T>
  void RemoveSetter(const std::string& name);
  
  void Log(const std::string& string);
  
  void GetListMessageHandler(const PartInterfaceGetListMessage<T>& message, const Theron::Address from);
  
  void Initialize(Part * part);
  
  static NameHash HashName(std::string name);

  protected:
  
  template <typename T>
  void InputMessageHandler(const PartInterfaceInputMessage<T>& message, const Theron::Address from);
  void MakeLinkMessageHandler(const PartInterfaceMakeLinkMessage& message, const Theron::Address from);
  void RemoveLinkMessageHandler(const PartInterfaceRemoveLinkMessage& message, const Theron::Addess from);
  void GetFieldMessageHandler(const PartInterfaceGetFieldMessage& message, const Theron::Address from);
  template <typename T>
  void SetFieldMessageHandler(const PartInterfaceSetFieldMessage<T>& message, const Theron::Address from);
  void SyncMessageHandler(const PartInterfaceSyncMessage& message, const Theron::Address from);
};

};
