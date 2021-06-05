// CForthMachineTest.h	: interface for the CForthMachineTest struct.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#if !defined(FORTHMACHINETEST_H)
#define FORTHMACHINETEST_H

#include "../UnitTest.h"
#include "../../forth/ForthMachine.h"
#include "../../forth/ForthConsole.h"

//////////////////////////////////////////////////////////////////////
// struct CForthMachineTest
//////////////////////////////////////////////////////////////////////

struct CForthMachineTest: public CUnitTest
{
	CForthMachineTest();

	void Run(bool);
};

//////////////////////////////////////////////////////////////////////
// class CBreakingEnvironment
//////////////////////////////////////////////////////////////////////

class CBreakingEnvironment:public CForthEnvironment
{
	const unsigned int myThreshold;

	TForthErrorCode myLastCode;
	int myCallCounter;

public:
	CBreakingEnvironment();

	unsigned int GetCounter()	{	return myCallCounter;	}
	unsigned int GetThreshold()	{	return myThreshold;	}

	void TakeControl(CForthMachine&);
	void Clear();
};

#endif // !defined(FORTHMACHINETEST_H)
