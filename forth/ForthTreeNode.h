// ForthTreeNode.h	: interface for the CForthTreeNode class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#if !defined(FORTHTREENODE_H)
#define FORTHTREENODE_H

#include "decl.h"

#include <map>
#include <set>
#include <string>

//////////////////////////////////////////////////////////////////////
// class CForthTreeNode
//////////////////////////////////////////////////////////////////////

class CForthTreeNode  
{
private:
	std::map<char, CForthTreeNode*> myBranches;
	CForthTreeNode* myParent;
	char myReference;

	std::set<int> myDefinitions;

public:
	CForthTreeNode(CForthTreeNode*, char);

	CForthTreeNode* GetBranch(char);
	CForthTreeNode* AddBranch(char);

	bool AddDefinition(int _item);
	int GetDefinition();
	void EraseDefinition(int);
	void Print(std::string&);

	virtual ~CForthTreeNode();
};

#endif // !defined(FORTHTREENODE_H)
