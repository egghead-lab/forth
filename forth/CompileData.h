// CompileData.h: interface for the CCompileData class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#if !defined(COMPILEDATA_H)
#define COMPILEDATA_H

#include "decl.h"
#include <vector>
#include <list>
#include <map>

//////////////////////////////////////////////////////////////////////
// class CCompileData
//////////////////////////////////////////////////////////////////////

class CCompileData  
{
	int myLevel;
	typedef std::vector<int> TItemVector;
	std::vector<TItemVector> myCycles;
	std::vector<int> myBasicLevels;
	std::vector<bool> myClosers;
	std::vector<int> myIterators;
	bool myBasicLevelSearch;

	CForthMachine* myForthMachine;
	typedef std::list<int> TItemList;
	std::vector<TItemList> myDefinitions;
	std::map<int, int> myCalls;

	int myMaxLevel;

public:
	CCompileData();

	int GetLevel()	{	return myLevel;	}
	void SetMachine(CForthMachine* _machine)	{	myForthMachine=_machine;	}	
	int GetCycle()	{	return myCycles.size()-1;	}
	bool Recursive(int _item)	{	return myCalls.find(_item)!=myCalls.end();	}
	void SetBasicLevelSearch(bool _search)	{	myBasicLevelSearch=_search;	}
	bool GetBasicLevelSearch()	{	return myBasicLevelSearch;	}
	int GetMaxLevel()	{	return myMaxLevel;	}

	void AddCycle();
	bool IsOpen();
	bool IsClose(int);
	void IncreaseLevels(int);
	void DecreaseLevels(int);
	void AddItems(int);
	void Iterate(int);
	int GetItem(int);
	int GetBasicLevel();
	void CloseCycle();
	void EndCycle();
	void AddDefinition(CDefinition*);
	void Clear();
	void ClearAll();
};

#endif // !defined(COMPILEDATA_H)
