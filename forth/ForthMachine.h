// ForthMachine.h	: interface for the CForthMachine class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#if !defined(FORTHMACHINE_H)
#define FORTHMACHINE_H

#include "decl.h"
#include "ForthTreeNode.h"
#include "ForthParser.h"

#include <map>
#include <string>
#include <vector>
#include <list>

//////////////////////////////////////////////////////////////////////
// class CForthMachine
//////////////////////////////////////////////////////////////////////

class CForthMachine  
{
	CForthEnvironment* myForthEnvironment;
	std::list<std::string> myCommandChain;
	const bool myEcho;

	CForthParser myParser;
	CForthTreeNode* myRoot;
	std::vector<int> myStack;
	std::vector<int> myVariables;
	std::map<int, CForthTreeNode*> myHistory;
	std::vector<CDefinition*> myDefinitions;
	std::list<int> myConditionStack;
	CCompileData* myCompileData;
	TForthErrorCode myStatus;
	bool myDefinitionStatus;
	int myNewDefinition;

	TWordFunction myPriorityFunction;

	static TForthErrorCode myAbort(CForthMachine*, int);
	static TForthErrorCode myReset(CForthMachine*, int);
	static TForthErrorCode myWords(CForthMachine*, int);
	static TForthErrorCode myForget(CForthMachine*, int);
	static TForthErrorCode myQuit(CForthMachine*, int);
	static TForthErrorCode myDefine(CForthMachine*, int);
	static TForthErrorCode myConstant(CForthMachine*, int);
	static TForthErrorCode myUseConstant(CForthMachine*, int);
	static TForthErrorCode myVariable(CForthMachine*, int);
	static TForthErrorCode myUseVariable(CForthMachine*, int);
	static TForthErrorCode myDereference(CForthMachine*, int);
	static TForthErrorCode myAssign(CForthMachine*, int);
	static TForthErrorCode myLimiter(CForthMachine*, int);
	static TForthErrorCode myPerform(CForthMachine*, int);
	static TForthErrorCode myIf(CForthMachine*, int);
	static TForthErrorCode myElse(CForthMachine*, int);
	static TForthErrorCode myThen(CForthMachine*, int);
	static TForthErrorCode myUseName(CForthMachine*, int);
	static TForthErrorCode myBegin(CForthMachine*, int);
	static TForthErrorCode myRepeat(CForthMachine*, int);
	static TForthErrorCode myUntil(CForthMachine*, int);
	static TForthErrorCode myWhile(CForthMachine*, int);

	int myForgetItem;

	void Init();
	void Restart();
	void Report(TForthErrorCode, int);
	TForthErrorCode ReportAndRestart(TForthErrorCode, int);
	TForthErrorCode ReportUndefined(std::string&);
	void Add(std::string&, const TWordFunction);
	void RecursiveSetUses(int);
	void AddDefinition(CDefinition*);
	CDefinition* GetDefinition(int);
	void DeleteDefinition(int);
	TForthErrorCode Perform(int);
	TForthErrorCode Recycle();

public:
	CForthMachine();
	CForthMachine(bool);
	CForthMachine(CForthEnvironment*);
	CForthMachine(CForthEnvironment*, bool);

	int GetForgetItem()	{	return myForgetItem;	}

	TForthErrorCode Run();
	void Do(std::string&);
	void Do(char*);

	// functions for unit testing
	void SetEnvironment(CForthEnvironment* _env)	{	myForthEnvironment=_env;	}
	unsigned int GetWordNumber()	{	return myHistory.size();	}
	int GetStackTop()	{	return myStack.back();	}
	int GetStackSize()	{	return myStack.size();	}
	int GetStackItem(int _i)	{	return myStack[_i];	}
	int GetMaxLevel();
	TForthErrorCode GetStatus();

	virtual ~CForthMachine();

	friend class CCompileData;
};

#endif // !defined(FORTHMACHINE_H)
