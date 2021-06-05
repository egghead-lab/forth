// UnitTest.h	: interface for the CUnitTest class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#if !defined(UNITTEST_H)
#define UNITTEST_H

//////////////////////////////////////////////////////////////////////
// class CUnitTest
//////////////////////////////////////////////////////////////////////

class CUnitTest  
{
	static unsigned int myComplete, myUserTerminated, myFail, myTotal;

public:
	static void Init();
	static void FinalizeTest(bool);
	static void Skip();
	static void Report();

	virtual void Run(bool);
};

#endif // !defined(UNITTEST_H)
