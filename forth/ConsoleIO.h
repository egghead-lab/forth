// ConsoleIO.h	: interface for the CConsoleIO class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#if !defined(CONSOLEIO_H)
#define CONSOLEIO_H

#include <string>

//////////////////////////////////////////////////////////////////////
// class CConsoleIO
//////////////////////////////////////////////////////////////////////

class CConsoleIO  
{
	std::string myCommand;

public:
	CConsoleIO();

	void GetCommand(std::string&);

	virtual ~CConsoleIO();
};

#endif // !defined(CONSOLEIO_H)
