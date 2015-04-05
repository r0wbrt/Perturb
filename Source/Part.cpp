#include <Perturb/Part.h>

namespace Perturb
{

  ContextToken Part::InputToken()
  {
    return this->message_token_;
  }
  
  void Part::__internal_set_message_token(ContextToken token)
  {
    this->message_token_ = token;
  }
  
  NameHash Part::InputHash()
  {
    return this->input_hash_;
  }
  
  void Part::__internal_set_input_hash(NameHash hash)
  {
    this->input_hash_ = hash;
  }
  
  Perturb::Address Part::FromAddress()
  {
    return this->message_address_;
  }
  
  void Part::__internal_set_message_address(Perturb::Address address)
  {
    this->message_address_ = address;
  }

  void Part::CheckTokens(bool value)
  {
    this->checking_token_ = value;
  }
   
  bool Part::__internal_is_checking_token()
  {
    return this->checking_token_;
  }
  
  ContextToken Part::Token()
  {
    return this->check_token_;
  }
  
  void Part::__internal_set_check_token(ContextToken token)
  {
    this->check_token_ = token;
  }
  
  ContextToken Part::__internal_get_check_token()
  {
    return this->check_token_;
  }
  
  
  void Part::Reset() 
  {/*NOOP*/}
  
  void Part::TokenChanged() 
  {/*NOOP*/}
  
  bool Part::DoWork()
  {/*NOOP*/}
  
  PartInterface& Part::Interface()
  {
    return *this->interface_;
  }
  
  Perturb::Application& Part::App()
  {
    return *this->application_;
  }

  void Part::__internal_initialize(PartInterface * interface)
  {
    this->interface_ = interface;
  }
  
  NameHash Part::GetFromOutput()
  {
    return this->output_source_hash_;
  }
  void Part::__internal_set_output_source_hash(NameHash output_source_hash)
  {
    this->output_source_hash_ = output_source_hash;
  }
  std::string Part::__internal_get_part_name()
  {
    return this->part_name_;
  }
  void Part::__internal_set_part_name(const std::string& name)
  {
    this->part_name_ = name;
  }
  

};
