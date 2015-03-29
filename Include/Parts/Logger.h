#include <iostream>
#include <perturb.h>
#include <atomic>
#include <Actor.h>
#include <App.h>
#include <string>

namespace Perturb { namespace Parts {

template <typename T>
class Logger : public Perturb::Actor
{
	private:	
		std::string pretty;
	protected:
		void WriteOutput(const T& val)
		{
			static std::atomic<bool> lock(false);
			while(std::atomic_exchange_explicit(&lock, true, std::memory_order_acquire))
					;
			std::cout << pretty << " :" << this->GetAddress().AsString() << ":" << this->getMessageFromAddress().AsString() << ":";
			std::cout << this->getMessageToken() << ":" << val << std::endl;
			std::atomic_store_explicit(&lock, false, std::memory_order_release);
			WriteToOutput<T>(val, 0, this->getMessageToken());
		}
	public:
		Logger(Perturb::App& App) : Perturb::Actor(App), pretty("No Name")
		{
			AddInputHandler(&Perturb::Parts::Logger<T>::WriteOutput, 0);
			this->ToggleTokenCheck();
			AddOutput<T>(0);
		}

		Logger( std::string pretty, Perturb::App& App) : Perturb::Actor(App), pretty(pretty) 
		{
			AddInputHandler(this, &Perturb::Parts::Logger<T>::WriteOutput, 0);
			this->ToggleTokenCheck();
			AddOutput<T>(0);
		}
	};

};};


		  
