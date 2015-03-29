#include <iostream>
#include <perturb.h>
#include <atomic>
#include <Actor.h>
#include <App.h>
#include <string>

namespace Perturb { namespace Parts {

/**
 * This part writes directly to the standard out. Has an optional pretty name that
 * can be set. Uses a mutex to serialize access to standard out.
 */
template <typename T>
class Logger : public Perturb::Actor
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
	
		/**
		 * Constructor, name defaults to 'No Name.'
		 * 
		 * @param App The application context this part executes in.
		 */
		Logger(Perturb::App& App) : Perturb::Actor(App), pretty_name_("No Name")
		{
			AddInputHandler(&Perturb::Parts::Logger<T>::WriteOutput, 0);
			this->ToggleTokenCheck();
			AddOutput<T>(0);
		}

		/**
		 *	Constructor, this one sets the pretty name of the logger.
		 * 
		 * @param pretty_name The pretty name used to make this logger identifiable 
		 * on the command line.
		 * @param App The application context that this part executes in. 
		 */
		Logger( std::string pretty_name, Perturb::App& App) : Perturb::Actor(App), pretty_name_(pretty_name) 
		{
			AddInputHandler(this, &Perturb::Parts::Logger<T>::WriteOutput, 0);
			this->ToggleTokenCheck();
			AddOutput<T>(0);
		}
	};

};};


		  
