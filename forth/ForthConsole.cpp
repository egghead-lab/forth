// Environment.cpp	: implementation of the CEnvironment class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ForthConsole.h"
#include "ForthMachine.h"

#include <conio.h>
#include <iostream>

//////////////////////////////////////////////////////////////////////
// class CForthConsole
//////////////////////////////////////////////////////////////////////

void CForthConsole::TakeControl(CForthMachine& _machine)
{
	std::string command;
	myConsole.GetCommand(command);
	_machine.Do(command);
}

void CForthConsole::Run()
{
	CForthMachine machine(this);

	for(TakeControl(machine); machine.Run()!=QUIT; TakeControl(machine))
		Sleep(1);
}
