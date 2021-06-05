// Definition.h	: interface for the CDefinition class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#if !defined(DEFINITION_H)
#define DEFINITION_H

#include "decl.h"

#include <vector>
#include <set>

//////////////////////////////////////////////////////////////////////
// class CDefinition
//////////////////////////////////////////////////////////////////////

class CDefinition  
{
private:
	int myItem;
	CForthTreeNode* myNode;
	TWordFunction myFunction;
	std::vector<int> myParamList;
	std::set<int> myUsers;
	std::set<int> myUses;
	int myLevel;

public:
	CDefinition(CForthTreeNode*, int, int);

	int GetItem()	{	return myItem;	}
	int GetLevel()	{	return myLevel;	}
	void SetNode(CForthTreeNode* _node)	{	myNode=_node;	}
	CForthTreeNode* GetNode()	{	return myNode;	}
	void AddParam(int _item)	{	myParamList.push_back(_item);	}
	void SetFunction(TWordFunction _function)	{	myFunction=_function;	}
	TWordFunction GetFunction()	{	return myFunction;	}
	unsigned int Size()	{	return myParamList.size();	}
	void AddUser(int _user)	{	myUsers.insert(_user);	}
	void AddUse(int _use)	{	myUses.insert(_use);	}
	std::set<int>& GetUses()	{	return myUses;	}
	std::set<int>& GetUsers()	{	return myUsers;	}
	void EraseUser(int _user)	{	myUsers.erase(_user);	}
	void ClearParams()	{	myParamList.clear();	}

	int GetParam(int);

	virtual ~CDefinition();
};

#endif // !defined(DEFINITION_H)
