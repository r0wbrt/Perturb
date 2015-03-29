#include <iostream>
#include <perturb.h>
#include <App.h>
#include <Parts/BinaryOperation.h>
#include <Parts/Logger.h>

int main()
{
	Perturb::App app;
	Perturb::Parts::Adder<double> add(app);
	Perturb::Parts::Logger<double> loginA(std::string("Logger In A"), app);
	Perturb::Parts::Logger<double> loginB(std::string("Logger In B"), app);
	Perturb::Parts::Logger<double> logout(std::string("Logger Out"), app);
		
	app.Link<double>(loginA.GetAddress(), add.GetAddress(), 0, 0);
	app.Link<double>(loginB.GetAddress(), add.GetAddress(), 0, 1);
	app.Link<double>(add.GetAddress(), logout.GetAddress(), 0, 0);

	app.SendToInput(77.0, 0, loginA.GetAddress());
	app.SendToInput(20000.0, 0, loginB.GetAddress());

	//app.SendToInput(1.0, 0, add.GetAddress());
	//app.SendToInput(2.0, 1, add.GetAddress());


	int a;
	std::cin >> a;
	return 0;
	


}
