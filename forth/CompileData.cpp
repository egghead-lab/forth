// CompileData.cpp	: implementation of the CCompileData class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CompileData.h"
#include "ForthMachine.h"
#include "Definition.h"

#include <assert.h>

//////////////////////////////////////////////////////////////////////
// class CCompileData
//////////////////////////////////////////////////////////////////////

CCompileData::CCompileData()
{
	myLevel=0;
	myDefinitions.resize(1);
	myBasicLevelSearch=false;
	myMaxLevel=0;
}

void CCompileData::AddCycle()
{
	assert(myLevel>=0);
	assert(myBasicLevels.size()>0?myBasicLevels.back()<=myLevel:true);
	myCycles.resize(myCycles.size()+1);
	myBasicLevels.push_back(myLevel);
	myClosers.push_back(false);
	myIterators.push_back(0);
}

bool CCompileData::IsOpen()
{
	if(myBasicLevels.size()>0 && myBasicLevels.back()==myLevel && !myClosers.back())
		return true;
	else
		return false;
}

bool CCompileData::IsClose(int _cycle)
{
	assert(_cycle>=0 && _cycle<=myClosers.size());
	if(_cycle==myClosers.size())
		return false;
	return myClosers[_cycle];
}

void CCompileData::IncreaseLevels(int _item)
{
	++myLevel;
	myDefinitions.resize(myDefinitions.size()+1);
	std::map<int, int>::iterator it=myCalls.find(_item);
	if(it==myCalls.end())
		myCalls[_item]=1;
	else
		++it->second;

	if(myLevel>myMaxLevel)
		myMaxLevel=myLevel;
}

void CCompileData::DecreaseLevels(int _item)
{
	assert(myForthMachine);
	TItemList& itemList=myDefinitions[myLevel];
	for(std::list<int>::reverse_iterator itr=itemList.rbegin(); itr!=itemList.rend(); ++itr)
	{
		CForthMachine::myForget(myForthMachine, myForthMachine->GetForgetItem());
		CForthMachine::myForget(myForthMachine, *itr);
	}

	std::map<int, int>::iterator it=myCalls.find(_item);
	assert(it!=myCalls.end());
	--it->second;
	if(it->second==0)
		myCalls.erase(it);			

	myDefinitions.pop_back();
	--myLevel;

	for(std::vector<int>::reverse_iterator ritl=myBasicLevels.rbegin(); ritl!=myBasicLevels.rend(); ++ritl)
	{
		if(*ritl>myLevel)
			*ritl=myLevel;
		else
			break;
	}
}

void CCompileData::AddItems(int _item)
{
	std::vector<TItemVector>::reverse_iterator ritc=myCycles.rbegin();
	std::vector<bool>::reverse_iterator ritCl=myClosers.rbegin();
	for(std::vector<int>::reverse_iterator ritl=myBasicLevels.rbegin(); ritl!=myBasicLevels.rend(); ++ritl)
	{
		if(*ritl<myLevel || *ritCl)
			break;
		ritc->push_back(_item);

		++ritc;
		++ritCl;
	}
}

void CCompileData::CloseCycle()
{
	myClosers.back()=true;
	myIterators.back()=-1;
	myBasicLevelSearch=true;
}

void CCompileData::Iterate(int _cycle)
{
	assert(_cycle<myIterators.size());
	++myIterators[_cycle];
}

int CCompileData::GetItem(int _cycle)
{
	assert(_cycle<myIterators.size());
	int it=myIterators[_cycle];
	return myCycles[_cycle][it];
}

int CCompileData::GetBasicLevel()
{
	assert(myBasicLevels.size()>0);
	return myBasicLevels.back();
}

void CCompileData::EndCycle()
{
	myCycles.pop_back();
	myBasicLevels.pop_back();
	myClosers.pop_back();
	myIterators.pop_back();
}

void CCompileData::AddDefinition(CDefinition* _def)
{
	assert(_def);
	myDefinitions.back().push_back(_def->GetItem());
}

void CCompileData::Clear()
{
	myCycles.clear();
	myBasicLevels.clear();
	myClosers.clear();
	myIterators.clear();
	myLevel=0;
	myDefinitions.resize(1);
	myBasicLevelSearch=false;
}

void CCompileData::ClearAll()
{
	assert(myLevel==0);
	myDefinitions.clear();
	myDefinitions.resize(1);
	myMaxLevel=0;
}


