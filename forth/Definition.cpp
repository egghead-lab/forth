// Definition.cpp	: implementation of the CDefinition class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Definition.h"

#include <iostream>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// class CDefinition
//////////////////////////////////////////////////////////////////////

CDefinition::CDefinition(CForthTreeNode* _node, int _item, int _level)
{
	myNode=_node;
	myItem=_item;
	myFunction=NULL;
	myLevel=_level;
}

int CDefinition::GetParam(int _item)
{
	assert(_item>=0 && _item<myParamList.size());
	return myParamList[_item];
}

CDefinition::~CDefinition()
{

}
