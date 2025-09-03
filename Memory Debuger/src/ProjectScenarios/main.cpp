#include <iostream>
#include "Scenarios.h"

// Pass 0-11 to choose the scenario
// IE: project2.exe 5
int main(int argc, char *argv[])
{
	int scenario = 1; // Or change this line to pick a scenario

	// Test Harness
	//======== BEGIN: DO NOT MODIFY THE FOLLOWING LINES =========//
	if (argc > 1)
	{
		scenario = std::atoi(argv[1]);
	}
	switch (scenario)
	{
		case 11: scenario_randompointer1(); break;
		case 10: scenario_randompointer2(); break;
		case 9:  scenario_vectornew_scalardelete(); break;
		case 8:  scenario_scalarnew_vectordelete(); break;
		case 7:  scenario_doubledelete(); break;
		case 6:  scenario_doublevectordelete(); break;
		case 5:  scenario_deletedmemoryread(); break;
		case 4:  scenario_deletedmemorywrite(); break;
		case 3:  scenario_readoverflow(); break;
		case 2:  scenario_writeoverflow(); break;
		case 1:  scenario_leaks(); break;
		default: scenario_good(); break;
	}
	//========  END: DO NOT MODIFY THE PREVIOUS LINES ===========//

	return 0;
}
