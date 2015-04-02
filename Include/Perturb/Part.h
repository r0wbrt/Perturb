#include <Perturb/Perturb.h>
#include <deque>

namespace Perturb {

class Application;
class PartInterface;
class Context;


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
  virtual bool Intialize() = 0; 
  virtual void SyncReply(const int SyncMessageId) {}
  PartInterface& Interface();
  void Lock();
  void UnLock();
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
  friend Context;
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
  void __internal_mark_in_use();
  void __internal_mark_not_in_use();
  void __internal_intialize(PartInterface * interface);
  void __internal_set_output_source_hash(NameHash output_source_hash);
  
  PartInterface * interface_;
  Perturb::Application * application_;
  Perturb::Address message_address_;
  ContextToken check_token_;
  ContextToken input_hash_;
  ContextToken message_token_;
  bool checking_token_;
  std::atomic_flag lock_ = ATOMIC_FLAG_INIT;
  NameHash output_source_hash_;
  

};

};
