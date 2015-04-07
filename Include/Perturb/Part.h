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
#ifndef __PERTURB_INCLUDE_PERTURB_PART_H__
#define __PERTURB_INCLUDE_PERTURB_PART_H__

#include <Perturb/Perturb.h>
#include <deque>
#include <string>
#include <atomic>

namespace Perturb {

class Application;
class PartInterface;
class PartFactory;


class Part
{
  public:
  
  protected:
  ContextToken InputToken();
  /**
   * Stores the hash of the input name the PartInterface routed the message to.
   * This function is to be used only by the PartInterface class. Use of this 
   * function inside of parts can produce undefined results and is strongly 
   * discouraged.
   *
   * @param hash The hash value of the input name. 
   */
  NameHash InputHash();
  /**
   * Gets the address of the part the input came from.
   */
  Perturb::Address FromAddress();
  void CheckTokens(bool value);
  ContextToken Token();
  virtual void Reset();
  virtual void TokenChanged();
  virtual bool DoWork();
  virtual void ListReply(const std::deque<std::pair<std::string, TypeHash> >& list, int id) {};
  virtual bool Initialize() = 0; 
  virtual void SyncReply(const int SyncMessageId) {}
  PartInterface& Interface();
  /**
   * Returns the hash of the input name currently handling the received input.
   *
   * @return The hash of the handling input name.
   */
  size_t GetFromOutput();
  Perturb::Application& App();

  private:
  /*Need friend class declarations becuase these methods below are 
    declared private to prevent use by derived parts. However, these methods
    are needed by these classes */
  friend PartInterface; 
  friend Application;
  friend PartFactory;
  /**
   * Stores the input token of the message. This function is to be used only by
   * the PartInterface class. Use of this function inside of parts can produce 
   * undefined results and is strongly discouraged.
   *
   * @param token The token of the received message.
   */
  void __internal_set_message_token(ContextToken token);
  
  /**
   * Returns the token value of the most recent input.
   *
   * @return The token value of the most recent input.
   */
  void __internal_set_input_hash(NameHash hash);
  
  /**
   * Sets the address the part input came from.
   *
   * @param address The address the message was sent from.
   */
  void __internal_set_message_address(Perturb::Address address);

   /**
   * Used by the part interface to determine if it should reject messages with 
   * a differing token value.
   *
   * @return True when token checking should be performed.
   */  
  bool __internal_is_checking_token();
  void __internal_set_check_token(ContextToken token);
  ContextToken __internal_get_check_token();
  void __internal_initialize(PartInterface * interface);
  void __internal_set_output_source_hash(NameHash output_source_hash);
  void __internal_set_application(Perturb::Application * app);
  
  
  PartInterface * interface_;
  Perturb::Application * application_;
  Perturb::Address message_address_;
  ContextToken check_token_;
  ContextToken input_hash_;
  ContextToken message_token_;
  bool checking_token_;
  NameHash output_source_hash_;
  

};

};

#endif
