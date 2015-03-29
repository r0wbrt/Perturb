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

#ifndef __PERTURB_INCLUDE_PARTS_BINARYOPERATION__H__
#define __PERTURB_INCLUDE_PARTS_BINARYOPERATION__H__

#include <App.h>
#include <Actor.h>
#include <deque>
#include <functional>


namespace Perturb { namespace Parts {

	template <typename T1, typename T2, typename T3 >
	class BinaryOperation : public Perturb::Actor
	{
		private:
			std::function<T3(T1&,T2&)> operator_function_;
			std::deque<T1> buffer_input_a_;
			std::deque<T2> buffer_input_b_;
		protected:
			void InputA(const T1& a)
			{
				this->buffer_input_a_.push_back(a);
			}
			void InputB(const T2& b)
			{
				this->buffer_input_b_.push_back(b);
			}
			void doWork()
			{
				while(this->buffer_input_a_.empty()!=true && this->buffer_input_b_.empty()!= true)
				{
					T1 a = this->buffer_input_a_.front();
					T2 b = this->buffer_input_b_.front();
					T3 c = this->operator_function_(a,b);
					this->WriteToOutput(c, 0);
					this->buffer_input_a_.pop_front();
					this->buffer_input_b_.pop_front();
				}
			}
			void Reset()
			{
				this->buffer_input_a_.clear();
				this->buffer_input_b_.clear();
			}
		public:
			BinaryOperation(std::function<T3(T1&,T2&)> operator_function, Perturb::App& App) : Actor(App), operator_function_(operator_function)
			{
				this->AddInputHandler(this, &Perturb::Parts::BinaryOperation<T1, T2, T3>::InputA, 0);
				this->AddInputHandler(this, &Perturb::Parts::BinaryOperation<T1, T2, T3>::InputB, 1);
				AddOutput<T3>(0);
			}

	};

	template <typename T>
	class Adder : public BinaryOperation<T,T,T>
	{
		public:
			Adder(Perturb::App& App) : BinaryOperation<T,T,T>::BinaryOperation([] (const T& a, const T& b) -> T 
				{
					return a + b;
				}, App)
		{	
		}
	};
	
	template <typename T>
	class Subtractor : public BinaryOperation<T,T,T>
	{
		public:
			Subtractor(Perturb::App& App) : BinaryOperation<T,T,T>::BinaryOperation([] (const T& a, const T& b) -> T 
				{
					return a - b;
				}, App)
		{	
		}
	};	
	
	template <typename T>
	class Mixer : public BinaryOperation<T,T,T>
	{
		public:
			Mixer(Perturb::App& App) : BinaryOperation<T,T,T>::BinaryOperation([] (const T& a, const T& b) -> T 
				{
					return a * b;
				}, App)
		{	
		}
	};	



		

};};

#endif
