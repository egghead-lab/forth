// HistoryRecord.h	: interface for the CHistoryRecord class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#if !defined(HISTORYRECORD_H)
#define HISTORYRECORD_H

#include "decl.h"

#include <map>
#include <vector>
#include <list>
#include <string>

//////////////////////////////////////////////////////////////////////
// class CHistoryRecord
//////////////////////////////////////////////////////////////////////

class CHistoryRecord  
{
private:
	std::map<char, CHistoryRecord*> myBranches;
	CHistoryRecord* myParent;
	char myReference;

	std::vector<int> myIntegerStack;
	typedef std::vector<TWordFunction> TCallChain;
	std::vector<TCallChain> myCallStack;

public:
	CHistoryRecord(CHistoryRecord*, char);

	CHistoryRecord* GetBranch(char);
	CHistoryRecord* AddBranch(char);

	void AddDefinition();
	void AddFunction(TWordFunction _function);
	void SetInteger(int);
	int GetInteger();
	void EraseDefinition();
	void Print(std::string&);

	virtual ~CHistoryRecord();
};

#endif // !defined(HISTORYRECORD_H)
