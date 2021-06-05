// ForthTreeNode.cpp: implementation of the CForthTreeNode class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//	
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ForthTreeNode.h"
#include "ForthMachine.h"

#include <assert.h>

//////////////////////////////////////////////////////////////////////
// class CForthTreeNode
//////////////////////////////////////////////////////////////////////

CForthTreeNode::CForthTreeNode(CForthTreeNode* _parent, char _reference)
{
	myParent=_parent;
	myReference=_reference;
}

CForthTreeNode* CForthTreeNode::GetBranch(char _key)
{
	std::map<char, CForthTreeNode*>::iterator it=myBranches.find(_key);
	if(it==myBranches.end())
		return NULL;

	return it->second;
}

CForthTreeNode* CForthTreeNode::AddBranch(char _key)
{
	assert(_key!=' ');
	std::map<char, CForthTreeNode*>::iterator it=myBranches.find(_key);
	if(it==myBranches.end())
	{
		CForthTreeNode* newNode=new CForthTreeNode(this, _key);
		myBranches[_key]=newNode;
		return newNode;
	}

	return it->second;
}

bool CForthTreeNode::AddDefinition(int _item)	
{	
	myDefinitions.insert(_item);	
	if(myDefinitions.size()>1)
	{
		std::string word;
		for(CForthTreeNode* cursor=this; cursor->myParent!=NULL; cursor=cursor->myParent)
			word=cursor->myReference+word;

		return true;
	}
	return false;
}

int CForthTreeNode::GetDefinition()
{
	if(myDefinitions.size()==0)
		return -1;
	return *(myDefinitions.rbegin());
}

void CForthTreeNode::EraseDefinition(int _item)
{
	if(myDefinitions.size()==0)
		return;

	assert(_item>=0);
	std::set<int>::iterator it=myDefinitions.find(_item);
	assert(it!=myDefinitions.end());
	myDefinitions.erase(it);
}

void CForthTreeNode::Print(std::string& _string)
{
	_string.assign(""); 
	for(CForthTreeNode* cursor=this; cursor->myParent!=NULL; cursor=cursor->myParent)
		_string.insert(0, cursor->myReference);
}

CForthTreeNode::~CForthTreeNode()
{
	for(std::map<char, CForthTreeNode*>::iterator it=myBranches.begin(); it!=myBranches.end(); ++it)
		delete it->second;
}
