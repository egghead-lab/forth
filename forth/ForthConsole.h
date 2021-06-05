// CForthConsole.h	: interface for the CForthConsole class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#if !defined(FORTHCONSOLE_H)
#define FORTHCONSOLE_H

#include "decl.h"
#include "ConsoleIO.h"

#include <string>

//////////////////////////////////////////////////////////////////////
// class CForthEnvironment
//////////////////////////////////////////////////////////////////////

class CForthEnvironment  
{
public:
	virtual void TakeControl(CForthMachine&) = 0;
};

//////////////////////////////////////////////////////////////////////
// class CForthConsole
//////////////////////////////////////////////////////////////////////

class CForthConsole:public CForthEnvironment
{
	CConsoleIO myConsole;

public:
	void TakeControl(CForthMachine&);
	void Run();
};

#endif // !defined(FORTHCONSOLE_H)
