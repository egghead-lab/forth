// ForthMachine.cpp	: implementation of the CForthMachine class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ForthMachine.h"
#include "CompileData.h"
#include "ForthConsole.h"
#include "Definition.h"
#include "../tools/Num2Str.h"

#include <iostream>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// class CForthMachine
//////////////////////////////////////////////////////////////////////

#define UNIQUE(n,d,s) if(n->AddDefinition(d) && s##myEcho)		\
	{	std::string word;										\
	n->Print(word);												\
	std::cout << " word " << word << " is not unique\n";	}

#define ASSERT													\
	assert(_machine);											\
	assert(_item>=0 && _item<_machine->myDefinitions.size());	\
	assert(_machine->myDefinitions[_item]!=NULL)


//////////////////////////////////////////////////////////////////////
//	static member functions

TForthErrorCode CForthMachine::myQuit(CForthMachine* _machine, int _item)
{
	ASSERT;

	if(_machine->myRoot)
		delete _machine->myRoot;
	_machine->myRoot=NULL;

	for(unsigned int i=0; i<_machine->myDefinitions.size(); ++i)
		if(_machine->myDefinitions[i])
			delete _machine->myDefinitions[i];
	_machine->myDefinitions.clear();
	_machine->myVariables.clear();

	_machine->myHistory.clear();
	_machine->Restart();

	if(_machine->myEcho)
		std::cout << "forth machine will now quit\n";
	return QUIT;
}

TForthErrorCode CForthMachine::myReset(CForthMachine* _machine, int _item)
{
	ASSERT;

	_machine->myHistory.clear();
	_machine->Init();
	return Ok;
}

TForthErrorCode CForthMachine::myAbort(CForthMachine* _machine, int _item)
{
	ASSERT;

	_machine->Restart();
	return Ok;
}

TForthErrorCode CForthMachine::myWords(CForthMachine* _machine, int _item)
{
	ASSERT;

	if(_machine->myEcho)
	{
		unsigned int count=0;
		for(std::map<int, CForthTreeNode*>::reverse_iterator it=_machine->myHistory.rbegin(); it!=_machine->myHistory.rend(); ++it)
		{
			std::string string;
			it->second->Print(string);
			std::cout << string;
			for(unsigned int i=0; i<16-string.size()%16; ++i)
				std::cout << " ";
			count+=1+string.size()/16;
		}
		if(count%5>0)
			std::cout << "\n";
	}

	return Ok;
}

TForthErrorCode CForthMachine::myForget(CForthMachine* _machine, int _item)
{
	ASSERT;

	if(_machine->myPriorityFunction)
	{
		assert(_machine->myPriorityFunction==myForget);
		CDefinition* definition=_machine->GetDefinition(_item);
		assert(definition);

		CForthTreeNode* node=definition->GetNode();
		int lastDef=node->GetDefinition();
		if(lastDef<0)
		{
			if(definition->GetFunction()==myUseName)
			{
				_machine->DeleteDefinition(_item);
				return _machine->ReportAndRestart(WRONG_LIST, _item);
			}
			else
				lastDef=_item;
		}

		CDefinition* def=_machine->myDefinitions[lastDef];
		def->EraseUser(lastDef);
		if(def->GetFunction()==myPerform)
		{
			std::set<int>& uses=def->GetUses();
			for(std::set<int>::iterator it=uses.begin(); it!=uses.end(); ++it)
			{
				CDefinition* usedDef=_machine->myDefinitions[*it];
				usedDef->EraseUser(lastDef);
				if(usedDef->GetUsers().size()==0)
				{
					if(usedDef->GetFunction()==myUseName)
						_machine->DeleteDefinition(*it);
					else
						myForget(_machine, *it);
				}
			}
		}

		_machine->myHistory.erase(lastDef);
		definition->GetNode()->EraseDefinition(lastDef);

		if(def->GetUsers().size()==0 && lastDef!=_machine->myForgetItem)
			_machine->DeleteDefinition(lastDef);

		_machine->myPriorityFunction=NULL;		
		return Ok;
	}
	
	_machine->myPriorityFunction=CForthMachine::myForget;
	return WAIT;
}

TForthErrorCode CForthMachine::myConstant(CForthMachine* _machine, int _item)
{
	ASSERT;

	if(_machine->myPriorityFunction)
	{
		assert(_machine->myPriorityFunction==myConstant);
		CDefinition* oldDefinition=_machine->GetDefinition(_item);
		assert(oldDefinition);

		CForthTreeNode* cursor=oldDefinition->GetNode();
		assert(cursor);

		int defNum=_machine->myDefinitions.size();
		CDefinition* definition=new CDefinition(cursor, defNum, _machine->myCompileData->GetLevel());
		definition->SetFunction(myUseConstant);
		definition->AddParam(_machine->myStack.back());
		definition->AddUser(defNum);

		if(cursor->AddDefinition(defNum))
		{
			std::string word;
			cursor->Print(word);
			std::cout << " word " << word << " is not unique\n";
		}
		_machine->AddDefinition(definition);

		_machine->myStack.pop_back();
		_machine->myHistory[defNum]=cursor;
		_machine->myPriorityFunction=NULL;
		
		return Ok;
	} 
	else if(_machine->myStack.size()==0)
		return _machine->ReportAndRestart(WRONG_STACK, _item);
	
	_machine->myPriorityFunction=CForthMachine::myConstant;
	return WAIT;
}

TForthErrorCode CForthMachine::myUseConstant(CForthMachine* _machine, int _item)
{
	ASSERT;

	CDefinition* definition=_machine->myDefinitions[_item];
	assert(definition->GetFunction()==myUseConstant);

	_machine->myStack.push_back(definition->GetParam(0));
	return Ok;
}

TForthErrorCode CForthMachine::myVariable(CForthMachine* _machine, int _item)
{
	ASSERT;

	if(_machine->myPriorityFunction)
	{
		assert(_machine->myPriorityFunction==myVariable);
		CDefinition* oldDefinition=_machine->GetDefinition(_item);
		assert(oldDefinition);

		CForthTreeNode* cursor=oldDefinition->GetNode();
		assert(cursor);

		int defNum=_machine->myDefinitions.size();
		CDefinition* definition=new CDefinition(cursor, defNum, _machine->myCompileData->GetLevel());
		definition->SetFunction(myUseVariable);
		definition->AddParam(_machine->myVariables.size());
		definition->AddUser(defNum);

		UNIQUE(cursor,defNum,_machine->);
		_machine->AddDefinition(definition);

		_machine->myVariables.resize(_machine->myVariables.size()+1);
		_machine->myHistory[defNum]=cursor;
		_machine->myPriorityFunction=NULL;
		
		return Ok;
	} 
	
	_machine->myPriorityFunction=CForthMachine::myVariable;
	return WAIT;
}

TForthErrorCode CForthMachine::myUseVariable(CForthMachine* _machine, int _item)
{
	ASSERT;

	CDefinition* definition=_machine->myDefinitions[_item];
	assert(definition->GetFunction()==myUseVariable);
	assert(definition->GetParam(0)<_machine->myVariables.size());

	_machine->myStack.push_back(definition->GetParam(0));
	return Ok;
}

TForthErrorCode CForthMachine::myDereference(CForthMachine* _machine, int _item)
{
	ASSERT;

	if(_machine->myStack.size()==0)
		return _machine->ReportAndRestart(WRONG_STACK, _item);

	int reference=_machine->myStack.back();
	_machine->myStack.pop_back();
	if(reference<0 || reference>=_machine->myVariables.size() || _machine->myVariables[reference]==NULL)
		return _machine->ReportAndRestart(WRONG_VALUE, _item);

	_machine->myStack.push_back(_machine->myVariables[reference]);
	return Ok;
}

TForthErrorCode CForthMachine::myAssign(CForthMachine* _machine, int _item)
{
	ASSERT;

	if(_machine->myStack.size()==0)
		return _machine->ReportAndRestart(WRONG_STACK, _item);

	int reference=_machine->myStack.back();
	_machine->myStack.pop_back();
	if(reference<0 || reference>=_machine->myVariables.size())
		return _machine->ReportAndRestart(WRONG_VALUE, _item);

	if(_machine->myStack.size()==0)
		return _machine->ReportAndRestart(WRONG_STACK, _item);

	int value=_machine->myStack.back();
	_machine->myStack.pop_back();
	_machine->myVariables[reference]=value;
	return Ok;
}

TForthErrorCode CForthMachine::myDefine(CForthMachine* _machine, int _item)
{
	ASSERT;

	if(_machine->myPriorityFunction)
	{
		assert(_machine->myPriorityFunction==myDefine);
		if(_machine->myDefinitionStatus)
		{
			CDefinition* oldDefinition=_machine->GetDefinition(_item);
			assert(oldDefinition);

			CForthTreeNode* cursor=oldDefinition->GetNode();
			assert(cursor);

			_machine->myNewDefinition=_machine->myDefinitions.size();
			CDefinition* definition=new CDefinition(cursor, _machine->myNewDefinition, _machine->myCompileData->GetLevel());
			definition->SetFunction(myPerform);
			_machine->AddDefinition(definition);
			_machine->myDefinitionStatus=false;

			return WAIT;
		}
		else
		{
			assert(_machine->myNewDefinition>=0);
			CDefinition* definition=_machine->myDefinitions[_machine->myNewDefinition];
			TWordFunction function=_machine->myDefinitions[_item]->GetFunction();

			if(function==myLimiter)
			{
				CForthTreeNode* node=definition->GetNode();
				UNIQUE(node,_machine->myNewDefinition,_machine->);
				_machine->myHistory[_machine->myNewDefinition]=node;

				if(definition->Size()==1 && _machine->myDefinitions[definition->GetParam(0)]->GetFunction()!=myUseName)
				{
					CDefinition* copyDef=_machine->myDefinitions[definition->GetParam(0)];
					definition->SetFunction(copyDef->GetFunction());
					definition->ClearParams();
					for(unsigned int i=0; i<copyDef->Size(); ++i)
						definition->AddParam(copyDef->GetParam(i));

					if(copyDef->GetFunction()==myPerform)
						_machine->RecursiveSetUses(_machine->myNewDefinition);
					else
						definition->AddUser(_machine->myNewDefinition);
				}				

				_machine->myPriorityFunction=NULL;
				_machine->myNewDefinition=-1;

				return Ok;
			}

			definition->AddParam(_item);
			return WAIT;
		}
	}

	assert(_machine->myNewDefinition<0);
	_machine->myPriorityFunction=CForthMachine::myDefine;
	_machine->myDefinitionStatus=true;

	return WAIT;
}

TForthErrorCode CForthMachine::myPerform(CForthMachine* _machine, int _item)
{
	ASSERT;

	_machine->myCompileData->IncreaseLevels(_item);

	CDefinition* definition=_machine->myDefinitions[_item];
	assert(definition->GetFunction()==myPerform);

	for(unsigned int i=0; i<definition->Size(); ++i)
	{
		int item=definition->GetParam(i);
		assert(item>=0 && item<_machine->myDefinitions.size());

		TForthErrorCode result=_machine->Perform(item);

		if(result!=Ok && result!=WAIT)
			return result;

		result=_machine->Recycle();
		if(result!=Ok && result!=WAIT)
			return result;

		if(_machine->myCompileData->GetBasicLevelSearch())
			break;
	}

	_machine->myCompileData->DecreaseLevels(_item);

	return Ok;
}

TForthErrorCode CForthMachine::myLimiter(CForthMachine* _machine, int _item)
{
	ASSERT;

	return _machine->ReportAndRestart(WRONG_LIST, _item);
}

TForthErrorCode CForthMachine::myIf(CForthMachine* _machine, int _item)
{
	ASSERT;

	CDefinition* definition=_machine->myDefinitions[_item];
	assert(definition->GetFunction()==myIf);
	assert(definition->Size()==0);

	if(_machine->myStack.size()==0)
		return _machine->ReportAndRestart(WRONG_STACK, _item);

	int value=_machine->myStack.back();
	_machine->myConditionStack.push_back(value==0?-1:0);
	_machine->myStack.pop_back();

	return WAIT;
}

TForthErrorCode CForthMachine::myElse(CForthMachine* _machine, int _item)
{
	ASSERT;

	if(_machine->myConditionStack.size()==0 || 
		(_machine->myConditionStack.back()!=0 && _machine->myConditionStack.back()!=-1))
		return _machine->ReportAndRestart(WRONG_LIST, _item);

	CDefinition* definition=_machine->myDefinitions[_item];
	assert(definition->GetFunction()==myElse);
	assert(definition->Size()<=1);

	int& value=_machine->myConditionStack.back();
	value=value==0?-1:0;

	return WAIT;
}

TForthErrorCode CForthMachine::myThen(CForthMachine* _machine, int _item)
{
	ASSERT;

	if(_machine->myConditionStack.size()==0 || 
		(_machine->myConditionStack.back()!=0 && _machine->myConditionStack.back()!=-1))
		return _machine->ReportAndRestart(WRONG_LIST, _item);

	_machine->myConditionStack.pop_back();
	return Ok;
}

TForthErrorCode CForthMachine::myUseName(CForthMachine* _machine, int _item)
{
	ASSERT;

	return _machine->ReportAndRestart(UNDEFINED, _item);
}

TForthErrorCode CForthMachine::myBegin(CForthMachine* _machine, int _item)
{
	ASSERT;

	_machine->myCompileData->AddCycle();
	_machine->myConditionStack.push_back(1);

	return WAIT;
}

TForthErrorCode CForthMachine::myWhile(CForthMachine* _machine, int _item)
{
	ASSERT;

	CDefinition* definition=_machine->myDefinitions[_item];
	assert(definition->GetFunction()==myWhile);
	assert(definition->Size()==0);

	if(_machine->myStack.size()==0 || _machine->myConditionStack.back()!=1)
		return _machine->ReportAndRestart(WRONG_STACK, _item);

	_machine->myConditionStack.pop_back();
	int value=_machine->myStack.back();
	_machine->myConditionStack.push_back(value==0?-2:2);
	_machine->myStack.pop_back();

	return Ok;
}

TForthErrorCode CForthMachine::myRepeat(CForthMachine* _machine, int _item)
{
	ASSERT;

	CDefinition* definition=_machine->myDefinitions[_item];
	assert(definition->GetFunction()==myRepeat);

	if(_machine->myConditionStack.size()==0 || (_machine->myConditionStack.back()!=1 && 
		_machine->myConditionStack.back()!=-2 && _machine->myConditionStack.back()!=2))
		return _machine->ReportAndRestart(WRONG_LIST, _item);

	if(_machine->myConditionStack.back()==-2)
	{
		_machine->myCompileData->EndCycle();
		_machine->myConditionStack.pop_back();
	}
	else
	{
		if(_machine->myForthEnvironment)
		{
			_machine->myForthEnvironment->TakeControl(*_machine);
			if(_machine->myStatus==ABORT)
				return ABORT;
		}

		_machine->myCompileData->CloseCycle();
		_machine->myConditionStack.back()=1;
	}

	return Ok;
}

TForthErrorCode CForthMachine::myUntil(CForthMachine* _machine, int _item)
{
	ASSERT;

	CDefinition* definition=_machine->myDefinitions[_item];
	assert(definition->GetFunction()==myUntil);

	if(_machine->myStack.size()==0 || _machine->myConditionStack.back()!=1)
		return _machine->ReportAndRestart(WRONG_STACK, _item);

	int value=_machine->myStack.back();
	_machine->myStack.pop_back();

	if(value!=0)
	{
		_machine->myCompileData->EndCycle();
		_machine->myConditionStack.pop_back();
	}
	else
	{
		if(_machine->myForthEnvironment)
		{
			_machine->myForthEnvironment->TakeControl(*_machine);
			if(_machine->myStatus==ABORT)
				return ABORT;
		}

		_machine->myCompileData->CloseCycle();
	}

	return Ok;
}

//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	private bound member functions

void CForthMachine::Report(TForthErrorCode _status, int _item)
{
	if(_status!=WAIT && myEcho)
	{
		if(_status==Ok)
		{
			std::string output;
			for(std::vector<int>::iterator it=myStack.begin(); it!=myStack.end(); ++it)
				output+=Num2String(*it);

			std::cout << "Ok ";
			if(output.size()>0)
				std::cout << "( " << output << ") ";
		}
		else 
		{
			std::string word;
			if(_item>=0)
				myDefinitions[_item]->GetNode()->Print(word);

			if(_status==UNDEFINED)
				std::cout << "word is undefined: " << word << ", abort\n";
			else if(_status==WRONG_STACK)
				std::cout << "wrong stack state for: " << word << ", abort\n";
			else if(_status==WRONG_LIST)
				std::cout << word << " is unexpected, abort\n";
			else if(_status==WRONG_VALUE)
				std::cout << "bad value in stack for: " << word << ", abort\n";
			else if(_status==OVERFLOW)
				std::cout << "call stack overflow, abort\n";
			else if(_status==ABORT)
				std::cout << "aborted\n";
		}

		std::cout << ">";
	}
}

TForthErrorCode CForthMachine::ReportAndRestart(TForthErrorCode _status, int _item)
{
	Report(_status, _item);
	Restart();
	return _status;
}

TForthErrorCode CForthMachine::ReportUndefined(std::string& _string)
{
	if(myEcho)
		std::cout << "word is undefined: " << _string << ", abort\n>";
	Restart();
	return UNDEFINED;
}

void CForthMachine::Add(std::string& _string, TWordFunction _function)
{
	CForthTreeNode* cursor=myRoot;
	for(std::string::iterator it=_string.begin(); it!=_string.end(); ++it)
		cursor=cursor->AddBranch(*it);
	CDefinition* definition=new CDefinition(cursor, myDefinitions.size(), 0);
	definition->SetFunction(_function);
	UNIQUE(cursor,myDefinitions.size(),this->);
	myHistory[myDefinitions.size()]=cursor;
	definition->AddUser(myDefinitions.size());
	if(_function==myForget)
		myForgetItem=myDefinitions.size();
	AddDefinition(definition);
}

void CForthMachine::AddDefinition(CDefinition* _def)
{
	assert(_def);
	assert(_def->GetItem()==myDefinitions.size());
	myDefinitions.push_back(_def);
	myCompileData->AddDefinition(_def);
}

CDefinition* CForthMachine::GetDefinition(int _item)
{
	if(_item<0 || _item>=myDefinitions.size())
		return NULL;
	return myDefinitions[_item];
}

void CForthMachine::RecursiveSetUses(int _item)
{
	CDefinition* definition=myDefinitions[_item];
	assert(definition->GetFunction()==myPerform);
		
	definition->AddUser(_item);
	for(unsigned int i=0; i<definition->Size(); ++i)
	{
		definition->AddUse(definition->GetParam(i));
		myDefinitions[definition->GetParam(i)]->AddUser(_item);
		std::set<int>& uses=myDefinitions[definition->GetParam(i)]->GetUses();
		for(std::set<int>::iterator it=uses.begin(); it!=uses.end(); ++it)
		{
			definition->AddUse(*it);
			myDefinitions[*it]->AddUser(_item);
		}
	}
}

void CForthMachine::DeleteDefinition(int _item)
{
	assert(_item>=0 && _item<myDefinitions.size());
	assert(myDefinitions[_item]!=NULL);

	CDefinition* definition=myDefinitions[_item];
	delete definition;
	myDefinitions[_item]=NULL;
	for(int i=myDefinitions.size()-1; i>=0 && myDefinitions[i]==NULL; --i)
		myDefinitions.pop_back();
}

TForthErrorCode CForthMachine::Perform(int _item)
{
	myCompileData->AddItems(_item);

	if(myPriorityFunction)
		return myPriorityFunction(this, _item);
	else
	{
		CDefinition* definition=myDefinitions[_item];
		TWordFunction function=definition->GetFunction();
		assert(function);

		if(function==myElse || function==myThen)
			return function(this, _item);
		else if(myConditionStack.size()>0 && myConditionStack.back()==-2 && function==myRepeat)
			return function(this, _item);
		else if(myConditionStack.size()==0 || myConditionStack.back()>=0)
		{
			if(function==myUseName)
			{
				int newDef=definition->GetNode()->GetDefinition();
				if(newDef>=0)
				{
					assert(newDef<myDefinitions.size());
					if(myDefinitions[newDef]->GetLevel()==myCompileData->GetLevel())
					{
						TWordFunction newFunction=myDefinitions[newDef]->GetFunction();
						return newFunction(this, newDef);
					}
					
					if(myCompileData->Recursive(newDef))
					{
						if(myForthEnvironment)
						{
							myForthEnvironment->TakeControl(*this);
							if(myStatus==ABORT)
								return ABORT;
						}

						if(myCompileData->GetLevel()>=100)
							return ReportAndRestart(OVERFLOW, _item);

						TWordFunction newFunction=myDefinitions[newDef]->GetFunction();
						return newFunction(this, newDef);
					}
				}

				std::string candidate;
				definition->GetNode()->Print(candidate);
				assert(candidate.size()>0);

				TNumberType numberType=myParser.CheckNumber(candidate);
				if(numberType==INTNUMBER)
				{
					int number=myParser.Convert2Int(candidate);
					myStack.push_back(number);
					return Ok;
				}
				else
					return ReportAndRestart(WRONG_LIST, _item);
			}
			else
				return function(this, _item);
		}
	}

	return WAIT;
}

TForthErrorCode CForthMachine::Recycle()
{
	int cycle=myCompileData->GetCycle();
	if(cycle>=0 && myCompileData->IsClose(cycle))
	{
		if(myCompileData->GetBasicLevelSearch())
		{
			if(myCompileData->GetLevel()>myCompileData->GetBasicLevel())
				return Ok;

			for(myCompileData->Iterate(cycle); ; myCompileData->Iterate(cycle))
			{
				myCompileData->SetBasicLevelSearch(false);
				int loopItem=myCompileData->GetItem(cycle);
				TForthErrorCode result=Perform(loopItem);
				if(result!=Ok && result!=WAIT)
					return result;

				if(!myCompileData->IsClose(cycle))
					break;
			}
		}
	}
	return Ok;
}

//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	public bound member functions

CForthMachine::CForthMachine():myEcho(true)
{
	myCompileData=NULL;
	myForthEnvironment=NULL;
	myRoot=NULL;
	Init();
	Report(Ok, -1);
}

CForthMachine::CForthMachine(bool _echo):myEcho(_echo)
{
	myCompileData=NULL;
	myForthEnvironment=NULL;
	myRoot=NULL;
	if(myEcho)
		std::cout << "\n";
	Init();
	Report(Ok, -1);
}

CForthMachine::CForthMachine(CForthEnvironment* _environment):myEcho(true)
{
	myCompileData=NULL;
	myForthEnvironment=_environment;
	myRoot=NULL;
	Init();
	Report(Ok, -1);
}

CForthMachine::CForthMachine(CForthEnvironment* _environment, bool _echo):myEcho(_echo)
{
	myCompileData=NULL;
	myForthEnvironment=_environment;
	myRoot=NULL;
	if(myEcho)
		std::cout << "\n";
	Init();
	Report(Ok, -1);
}

void CForthMachine::Init()
{
	if(myRoot)
		delete myRoot;

	for(unsigned int i=0; i<myDefinitions.size(); ++i)
		if(myDefinitions[i])
			delete myDefinitions[i];
	myDefinitions.clear();

	myVariables.clear();

	if(myCompileData==NULL)
		myCompileData=new CCompileData();
	else
		myCompileData->ClearAll();

	myRoot=new CForthTreeNode(NULL, ' ');
	Restart();

	std::string exit="QUIT";
	Add(exit, myQuit);
 
	std::string reset="RESET";
	Add(reset, myReset);

	std::string abort="ABORT";
	Add(abort, myAbort);

	std::string words="WORDS";
	Add(words, myWords);

	std::string forget="FORGET";
	Add(forget, myForget);

	std::string constant="CONSTANT";
	Add(constant, myConstant);

	std::string variable="VARIABLE";
	Add(variable, myVariable);

	std::string dereference="@";
	Add(dereference, myDereference);

	std::string assign="!";
	Add(assign, myAssign);

	std::string define=":";
	Add(define, myDefine);

	std::string limiter=";";
	Add(limiter, myLimiter);

	std::string iF="IF";
	Add(iF, myIf);

	std::string eLSE="ELSE";
	Add(eLSE, myElse);
	
	std::string tHEN="THEN";
	Add(tHEN, myThen);

	std::string begin="BEGIN";
	Add(begin, myBegin);

	std::string wHILE="WHILE";
	Add(wHILE, myWhile);

	std::string rEPEAT="REPEAT";
	Add(rEPEAT, myRepeat);

	std::string uNTIL="UNTIL";
	Add(uNTIL, myUntil);

	if(myEcho)
		std::cout << "forth machine is initialized with " << myHistory.size() << " words\n";

	myStatus=Ok;
}

void CForthMachine::Restart()
{
	myStack.clear();
	myConditionStack.clear();
	myCompileData->Clear();
	myPriorityFunction=NULL;
	myNewDefinition=-1;
	myStatus=Ok;
}

void CForthMachine::Do(std::string& _string)
{
	std::vector<std::string>& string=myParser.Parse(_string);

	for(std::vector<std::string>::iterator it=string.begin(); it!=string.end(); ++it)
	{
		if(*it=="ABORT")
		{
			Restart();
			myStatus=ABORT;
		}
		myCommandChain.push_back(*it);
	}
}

void CForthMachine::Do(char* _str)
{
	std::string input;
	input.assign(_str);
	Do(input);
}

TForthErrorCode CForthMachine::Run()
{
	if(myCommandChain.size()==0)
		return WAIT;

	myCompileData->SetMachine(this);

	for(std::list<std::string>::iterator it=myCommandChain.begin(); it!=myCommandChain.end(); it=myCommandChain.begin())
	{
		std::string& word=*it;
		CForthTreeNode* cursor=myRoot;
		for(std::string::iterator its=word.begin(); its!=word.end() && cursor!=NULL; ++its)
			cursor=cursor->GetBranch(*its);

		int item=cursor?cursor->GetDefinition():-1;
		if(item<0)
		{
			bool skip=false;
			if(myPriorityFunction || myCompileData->IsOpen())
			{
				if(cursor==NULL)
				{
					cursor=myRoot;
					for(std::string::iterator its=word.begin(); its!=word.end(); ++its)
						cursor=cursor->AddBranch(*its);
				}

				item=myDefinitions.size();
				CDefinition* definition=new CDefinition(cursor, item, 0);
				definition->SetFunction(myUseName);
				myDefinitions.push_back(definition);
			}

			if(item<0)
			{
				if(myConditionStack.size()==0 || myConditionStack.back()>=0)
				{
					TNumberType numberType=myParser.CheckNumber(word);
					if(numberType==INTNUMBER)
					{
						int number=myParser.Convert2Int(word);
						myStack.push_back(number);
						myStatus=Ok;
						myCommandChain.pop_front();
						continue;
					}
				}
				else
				{
					myCommandChain.pop_front();
					continue;
				}
			}
		}
		
		if(item>=0)
			myStatus=Perform(item);
		else
			myStatus=ReportUndefined(word);

		if(myStatus!=Ok && myStatus!=WAIT)
		{
			myCommandChain.clear();
			break;
		}
		else
		{
			Recycle();
			if(myStatus==ABORT)
			{
				for(std::list<std::string>::iterator it=myCommandChain.begin(); it!=myCommandChain.end(); it=myCommandChain.begin())
				{
					std::string command=*it;
					if(command!="ABORT")
						myCommandChain.pop_front();
					else
						break;
				}
			}
		}

		myCommandChain.pop_front();
	}

	if(myStatus==Ok && myConditionStack.size()==0)
		Report(Ok, -1);
	else if(myStatus==ABORT)
		Report(ABORT, -1);

	return myStatus;
}

TForthErrorCode CForthMachine::GetStatus()
{
	if(myStatus==Ok && myConditionStack.size()!=0)
		return WAIT;
	return myStatus;
}

int CForthMachine::GetMaxLevel()	
{	
	return myCompileData->GetMaxLevel();	
}

CForthMachine::~CForthMachine()
{
	if(myRoot)
		delete myRoot;

	for(unsigned int i=0; i<myDefinitions.size(); ++i)
		if(myDefinitions[i])
			delete myDefinitions[i];

	if(myCompileData)
		delete myCompileData;
}

//
//////////////////////////////////////////////////////////////////////

#undef UNIQUE