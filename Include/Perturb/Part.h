#include <Perturb/Perturb.h>

namespace Perturb {

class PartInterface;
class Context;


class Part
{
  public:

  protected:
  int InputToken();
  /**
   * Stores the hash of the input name the PartInterface routed the message to.
   * This function is to be used only by the PartInterface class. Use of this 
   * function inside of parts can produce undefined results and is strongly 
   * discouraged.
   *
   * @param hash The hash value of the input name. 
   */
  size_t InputHash();
  /**
   * Gets the address of the part the input came from.
   */
  Perturb::Address FromAddress();
  void CheckTokens(bool value);
  int Token();
  virtual void Reset();
  virtual void TokenChanged();
  virtual bool DoWork();
  virtual void Intialize(); 
  Interface& PartInterface();
  void Lock();
  void UnLock();
  size_t GetFromOutput();

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
  void ___internal_set_message_token(int token);
  /**
   * Returns the token value of the most recent input.
   *
   * @return The token value of the most recent input.
   */
  void ___internal_set_input_hash(size_t hash);
  /**
   * Sets the address the part input came from.
   *
   * @param address The address the message was sent from.
   */
  void ___internal_set_message_address(Perturb::Address address);
  /**
   * Returns the hash of the input name currently handling the received input.
   *
   * @return The hash of the handling input name.
   */
   /**
   * Used by the part interface to determine if it should reject messages with 
   * a differing token value.
   *
   * @return True when token checking should be performed.
   */  
  bool __internal_is_checking_token();
  int __internal_set_check_token(int token);
  int __internal_get_check_token();
  void __internal_mark_in_use();
  void __internal_mark_not_in_use();
  void __internal_intialize(PartInterface * interface);
  void __internal_set_output_source_hash(size_t output_source_hash);
  
  PartInterface * interface_;
  Perturb::Address message_address_;
  int check_token_;
  size_t input_hash_;
  int message_token_;
  bool checking_token_;
  bool in_use_ = false;
  size_t output_source_hash_;
  

};

};
