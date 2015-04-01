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
#ifndef __PERTURB_INCLUDE_PERTURB_PARTINTERFACE_H__
#define __PERTURB_INCLUDE_PERTURB_PARTINTERFACE_H__

/**
 * This file holds the definitions of the PartInterface Class as well as the 
 * code for the template functions. All non-template functions are declared
 * in the associated PartInterface.cpp file.
 *
 * Creator: Robert Taylor
 * Contributors: Robert Taylor 
 * Modified in Version: 0.2.00
 */

#include <Theron/Theron.h>
#include <Perturb/Perturb.h>
#include <Perturb/Part.h>
#include <Perturb/PartMessages.h>
#include <deque>
#include <unordered_map>
#include <typeinfo>
#include <algorithm>

namespace Perturb {

class Part;


/**
 * Part interface abstracts parts from the underlying system used to move input
 * and output data. This class must be created using the Application class to
 * ensure proper intialization.
 * @author Robert Taylor
 */
class PartInterface : public Theron::Actor
{
  private:
  /*These typedefs are used to help reduce the number of > when writing code*/
  typedef std::function<const void(void *)> FunctionPointer;
  typedef std::pair<NameHash, Perturb::Address> ToEntry;
  typedef std::deque<ToEntry> ToList;
  typedef std::unordered_map<NameHash, FunctionPointer> InputMap;
  typedef std::unordered_map<NameHash, ToList> OutputMap;
  typedef std::pair<std::string, TypeHash> IOEntry;
  
  /**A pointer to the part_ object the interface provides services to*/
  Part * part_; 
  /**A multidimensional map used to store input handlers by name and type*/
  std::unordered_map<TypeHash, InputMap> input_map_;
  /**A multidemensional map used to store output subscription list by name and type*/
  std::unordered_map<TypeHash, OutputMap> output_map_;
  /**A list of all inputs exposed by the part*/
  std::deque<IOEntry> input_name_list_;
  /**A list of all outputs exposed by the part*/
  std::deque<IOEntry> output_name_list_;
  /**The address where all error messages and console output should be sent to*/
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
        this->LogError("Register input failed in AddInputHandler.");
        return false; /*No point in continuing if we don't have an handler to 
                          receive a message*/
      }
    }
    /*Insert the name and type into the global input list for diagnostic 
       purposes*/
    IOEntry ioentry(name, type_hash);
    this->input_name_list_.push_back(ioentry);
    /*Wrap the call back in a lambda function so the callback can be stored
      as a generic function with predictable type signature.*/
    this->input_map_[type_hash][this->HashName(name)] = [callback, owner] (const void * p)
    -> void
    {
      owner->callback((T*)p);
    };
    
    return true;
  }
  
  
  template <typename T>
  bool RemoveInputHandler(const std::string& name)
  {
    static TypeHash type_hash = typeid(T).hash_code();
    /*Check if this type exist in the input_map_*/
    if(this->input_map_.count(type_hash)!=1)
      return false;
    
    /*Ensure this input name exist*/
    if(this->input_map_[type_hash].count(this->HashName(name))!=1)
      return false;
    
    /*Compiler throws warning when capturing static type*/
    TypeHash type = type_hash; 
    /*Remove this input from the input_name_list_*/  
    auto it = std::find_if(this->input_name_list_.begin(), this->input_name_list_.end(), 
    [name, type] (const IOEntry value) -> bool 
    {
      if((value.first == name) && (value.second == type))
      {
        return true;
      } else {
        return false;
      }
    });
      
    /*Check to see if we have found the input pair then delete the entry*/  
    if(((*it).first == name) && ((*it).second == type_hash))
      this->input_name_list_.erase(it);
      
    /*Remove input entry from the map*/ 
    this->input_map_[type_hash].erase(this->HashName(name));
    
    /*If no more inputs are registered with this type we might as well
      remove the message handler and associated list to free memory*/ 
    if(this->input_map_[type_hash].empty()== true)
    {
    /*Only remove the input map of this type if we can deregister our handler*/
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
    /*Create a new output list and insert into the output map with a
    hash of name as key*/
    this->output_map_[type_hash][this->HashName(name)] = list;
    
    /*Insert a new output entry into our output list*/
    IOEntry entry(this->HashName(name), type_hash);
    this->output_name_list_.push_back(entry);
    
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
      
    TypeHash type = type_hash;
    /*Find and remove from ouput_name_list_*/  
    auto it = std::find_if(this->output_name_list_.begin(), this->output_name_list_.end(),
    [name, type] (const IOEntry value) -> bool 
    {
      if((value.first == name) && (value.second == type))
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
    if(this->output_map_[type_hash].empty() == true)
    {
      this->output_map_.erase(type_hash);
    }
    
    return true;
  }
  
    /**
   * An overloaded version of WriteToOutput(value, Name Hash, Token).
   *
   * @param The value to write and send to other parts connected to this output.
   * @param name_hash Hash of the name of the output to write to.
   * @param Returns true on when no unrecoverable errors occur. A return of 
   * true does not guarantee all output messages were succesfully delivered
   * to a part.
   */
  template <typename T>
  bool WriteToOutput(const T& value, const NameHash name_hash)
  {
    return this->WriteToOutput<T>(value, name_hash, this->part_->__internal_get_check_token());
  }
    /**
   * An overloaded version of WriteToOutput(value, Name Hash).
   *
   * @param The value to write and send to other parts connected to this output.
   * @param name The name of the output to write to.
   * @param token The context token to apply to all output messages.
   * @param Returns true on when no unrecoverable errors occur. A return of 
   * true does not guarantee all output messages were succesfully delivered
   * to a part.
   */
  template <typename T>
  bool WriteToOutput(const T& value, const std::string& name, const ContextToken token)
  {
    return this->WriteToOutput<T>(value, this->HashName(name), token);
  }
  /**
   * An overloaded version of WriteToOutput(value, Name Hash).
   *
   * @param The value to write and send to other parts connected to this output.
   * @param name The name of the output to write to.
   * @param Returns true on when no unrecoverable errors occur. A return of 
   * true does not guarantee all output messages were succesfully delivered
   * to a part.
   */
  template <typename T>
  bool WriteToOutput(const T& value, const std::string& name)
  {
    /*Redirect to avoid code duplication*/
    return this->WriteToOutput<T>(value, this->HashName(name), this->part_->__internal_get_check_token());
  }
  
  /**
   * The part interface class manages a list per by type then by output name for
   * each part. The list has entries describing where to send an output value.
   * Calling this function sends the output value to each part on the list. 
   *
   * @param value This is what to send to over the output to other parts.
   * @param hash The name hash identifying this output
   * @param token The context token to attach to the outgoing messages.
   * @return Returns true on when no unrecoverable errors occur. A return of 
   * true does not guarantee all output messages were succesfully delivered
   * to a part. Parts interested in a syncronous reply will have to register
   * a method of receiving message acknolegments. 
   */
  template <typename T>
  bool WriteToOutput(const T& value, const NameHash hash, const ContextToken token)
  {
    static TypeHash type_hash = typeid(T).hash_code();
    
    /*Check if an output exist with this type*/
    if(this->output_map_.count(type_hash) != 1)
      return false;
    
    /*Check if an output exist with this name hash*/
    if(this->output_map_[type_hash].count(hash) != 1)
      return false;
     
    ToList& list = this->output_map_[type_hash][hash];
    
    TypeHash type = type_hash;
    /*Send out the messages*/
    std::for_each(list.begin(), list.end(), 
    [&value, hash, token, this, type] (const ToEntry& entry)
    {
      PartInterfaceInputMessage<T> message;
      message.type = type;
      message.input_hash = entry.first;
      message.output_hash = hash;
      message.payload = value;
      message.token = token;
      this->Send(message, entry.second);
    });
    
    return true;
  }


  template <typename T, typename C>
  void AddGetter(C * owner, T (C::*callback)(), const std::string& name);
  template <typename T, typename C>
  void AddSetter(C * owner, void (C::*callback)(const T& value), const std::string& name);
  
  template <typename T>
  void RemoveGetter(const std::string& name);
  template <typename T>
  void RemoveSetter(const std::string& name);
  
  void Log(const std::string& string);
  void LogError(const std::string& string);
  
  void Initialize(Part * part, const Perturb::Address log_address);
  
  static NameHash HashName(std::string name);

  protected:
  
  void GetListMessageHandler(const PartInterfaceGetListMessage& message, const Perturb::Address from);
  template <typename T>
  void InputMessageHandler(const PartInterfaceInputMessage<T>& message, const Theron::Address from);
  void MakeLinkMessageHandler(const PartInterfaceMakeLinkMessage& message, const Theron::Address from);
  void RemoveLinkMessageHandler(const PartInterfaceRemoveLinkMessage& message, const Theron::Address from);
  void GetFieldMessageHandler(const PartInterfaceGetFieldMessage& message, const Theron::Address from);
  template <typename T>
  void SetFieldMessageHandler(const PartInterfaceSetFieldMessage<T>& message, const Theron::Address from);
  void SyncMessageHandler(const PartInterfaceSyncMessage& message, const Theron::Address from);
};

};

#endif
