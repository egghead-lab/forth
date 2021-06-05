// ForthC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "unittests/UnitTest.h"
#include "unittests/units/ForthMachineTest.h"
#include "forth/ForthConsole.h"

#define UNITTEST(a)		\
	test=new a();		\
	test->Run(echo);	\
	delete test		

int main(int argc, char* argv[])
{
	CUnitTest::Init();

	bool echo=false;
	CUnitTest* test=NULL;

	UNITTEST(CForthMachineTest);

	CForthConsole console;
	console.Run();

	return 0;
}

