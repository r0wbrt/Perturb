#include <Perturb/Part.h>

namespace Perturb
{
  void Part::set_message_token(int token)
  {
    this->message_token_ = token;
  }
  int Part::get_message_token()
  {
    return this->message_token_;
  }
  void Part::set_input_hash(size_t hash)
  {
    this->input_hash_ = hash;
  }
  size_t Part::get_input_hash()
  {
    return this->input_hash_;
  }
  void Part::set_message_address(Perturb::Address address)
  {
    this->message_address_ = address;
  }
  void Part::get_message_address()
  {
    return this->message_address_;
  }
  bool Part::is_checking_token()
  {
    return this->checking_token_;
  }
  bool Part::toggle_checking_token()
  {
    return this->checking_token_ = !this->checking_token_;
  }
  void Part::set_interface(PartInterface * interface)
  {
    this->interface_ = interface;
  }
  PartInterface * Part::get_interface()
  {
    return this->interface_;
  }
  int Part::set_check_token(int token)
  {
    this->check_token_ = token;
  }
  int Part::get_check_token()
  {
    return this->check_token_;
  }
  void Part::mark_in_use()
  {
    this->in_use_ = true; 
  }
  void Part::mark_not_in_use()
  {
    this->in_use_ = false;
  }
  bool Part::is_in_use()
  {
    return this->in_use_;
  }
  
  virtual void Part::Reset() {}
  virtual void Part::TokenChanged() {}
  virtual bool Part::DoWork() {}
  virtual void Part::Intialize() {}
};
