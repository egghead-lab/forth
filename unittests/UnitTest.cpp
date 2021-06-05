// UnitTest.cpp	: implementation of the CUnitTest class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UnitTest.h"
#include <iostream>

//////////////////////////////////////////////////////////////////////
// class CUnitTest
//////////////////////////////////////////////////////////////////////

unsigned int CUnitTest::myComplete;
unsigned int CUnitTest::myUserTerminated; 
unsigned int CUnitTest::myFail; 
unsigned int CUnitTest::myTotal;

void CUnitTest::Init()
{
	std::cout << "Unit testing\n";
	myComplete=0;
	myUserTerminated=0;
	myFail=0;
	myTotal=0;
}

void CUnitTest::Skip()
{
	++myUserTerminated;
	++myTotal;
}

void CUnitTest::FinalizeTest(bool _result)
{
	if(_result)
	{
		std::cout << " COMPLETE\n";
		++myComplete;
	}
	else
	{
		std::cout << " FAIL\n";
		++myFail;
	}

	++myTotal;
}

void CUnitTest::Report()
{
	std::cout << "Unit testing done\n";
	if(myTotal>0)
	{
		if(myComplete>0)
			std::cout << myComplete << " COMPLETE | ";
		if(myUserTerminated>0)
			std::cout << myUserTerminated << " TERMINATED | ";
		if(myFail>0)
			std::cout << myFail << " FAILED | ";
		std::cout << myTotal << " TOTAL\n\n";
	}
}

void CUnitTest::Run(bool _echo)
{
	std::cout << " Blank test";
	if(_echo)
		std::cout << std::endl;
	FinalizeTest(true);
}
