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

#include "perturb.h"

/*A 2 input simple adding-Multiplying machine

   (toSummerA)-->  inputA)-- 
                            \
                           ( + ) ---> (x) ----> MachineOut
                            /          |
   (toSummerB)--> (inputB)--           |
                                      /
                       toMulB--------

(A+B)*C is the mathematical equivalence.
*/


int main()
{

  int ToSummerA = 0;
  int ToSummerB = 1;
  int ToMulB = 2;
  int Output = 3;
  int k = 0;

  Perturb::App app;
  Perturb::AdderActor adder(app);
  Perturb::MultiplierActor mul(app);
  Perturb::DataSink<double> sink(app);
  Perturb::DataSource<double> source(app);
  
  sink.makeInput(Output);
  source.makeOutput(ToSummerA);
  source.makeOutput(ToSummerB);
  source.makeOutput(ToMulB);
  
  Perturb::MakeLink<double>(adder, Perturb::AdderActor::Output, mul, Perturb::MultiplierActor::InputA);
  Perturb::MakeLink<double>(app, ToMulB, mul, Perturb::MultiplierActor::InputB);
  Perturb::MakeLink<double>(mul, Perturb::MultiplierActor::Output, sink, 0);
  
  for(int i = 0; i < 15; i++)
  {
    app.WriteToOutput<double>(ToSummerA, i);
    app.WriteToOutput<double>(ToSummerB, 2*i); 
    app.WriteToOutput<double>(ToMulB, 3*i);
    if(app.isInputEmpty<double>(Output)!=false)
    {
      std::cout << app.ReadFromInput<double>(Output) << std::endl;
      k++;
    }
 }

  for(int i = k; i < 15; i++)
  {
    app.wait();
    std::cout << app.ReadFromInput<double>(Output) << std::endl;
  }
  

  return 0;
}
