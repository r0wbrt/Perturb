#include <iostream>
#include <perturb.h>
#include <atomic>
#include <Actor.h>
#include <App.h>
#include <string>

namespace Perturb { namespace Parts {

/**
 * This part sends messages to the log. Has an optional pretty name that
 * can be set.
 */
template <typename T>
class Logger : public Perturb::Part
{
	private:	
		std::string pretty_name_; /*The name identifying this logger*/
	protected:
	
		/*
		Handles a received message. Write the messages from address, token, and 
		value to standard out. This then passes the message on to its output 0. 
		*/
		void WriteOutput(const T& val)
		{
			static std::atomic<bool> lock(false);
			
			/*Critical Section Starts*/
				while(std::atomic_exchange_explicit(&lock, true, std::memory_order_acquire))
						{ /*Spin Lock until we can aquire the mutex*/ }
				std::cout << pretty_name_ << ". (" << this->GetAddress().AsString() << ")" << std::endl;
				std::cout << "From Address: " << this->get_message_from_address().AsString() << std::endl;
				std::cout << "Token: " << this->get_message_token() << std::endl;
				std::cout << "Value: " << val << std::endl << std::endl;
				std::atomic_store_explicit(&lock, false, std::memory_order_release);
			/*Critical Section Ends*/
			
			WriteToOutput<T>(val, 0, this->get_message_token());
		}
	public:
	
	  
	
	  bool Intialize()
	  {
	    this->CheckTokens(false);
	    this->PartInterface().AddInputHandler(this, Perturb::Parts::Logger::WriteOutput<T>, "Log In");
	    this->PartInterface().AddOutput<T>("Log Out");
	  }
	  
	  template <typename T>
	  static * Logger Create_Logger()
	  {
	    return new Logger<T>();
	  }
	};

};};


		  
