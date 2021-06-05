// ConsoleIO.cpp: implementation of the CConsoleIO class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConsoleIO.h"

#include <iostream>
#include <conio.h>

//////////////////////////////////////////////////////////////////////
// class CConsoleIO
//////////////////////////////////////////////////////////////////////

CConsoleIO::CConsoleIO()
{

}

void CConsoleIO::GetCommand(std::string& _string)
{
	for(; kbhit(); )
	{
		char c=getch();
		if(c==13)
		{
			std::cout << std::endl;
			if(myCommand.size()>0)
			{
				_string=myCommand;
				myCommand="";
				break;
			}
		}
		else if(c==8 && myCommand.size()>0)
		{
			std::cout << "\b \b";
			myCommand.resize(myCommand.size()-1);
		}
		else
		{
			std::cout << c << std::flush;
			myCommand+=c;
		}
	}
}

CConsoleIO::~CConsoleIO()
{

}
