// ForthParser.cpp	: implementation of the CForthParser class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ForthParser.h"

#include <assert.h>

//////////////////////////////////////////////////////////////////////
// class CForthParser
//////////////////////////////////////////////////////////////////////

CForthParser::CForthParser()
{

}

std::vector<std::string>& CForthParser::Parse(std::string _input)
{
	myOutput.clear();
	bool myWordContinues=false;

	for(std::string::iterator it=_input.begin(); it!=_input.end(); ++it)
	{
		if(*it!=' ' && *it!='\n' && *it!='\t')
		{
			if(myWordContinues==false)
			{
				myWordContinues=true;
				myOutput.resize(myOutput.size()+1);
			}
			myOutput.back()+=(*it);
		}
		else if(myWordContinues)
			myWordContinues=false;

	}

	return myOutput;
}

TNumberType CForthParser::CheckNumber(std::string& _input)
{
	if(_input.size()==0)
		return NOTANUMBER;			// нет цифр

	unsigned int i=0;
	if(_input[i]=='-')
	{
		if(_input.size()==1)
			return NOTANUMBER;		// только минус
		else
			++i;					// проверяем остальные знаки
	}

	if(_input[i]=='0' && _input.size()>i+1 && _input[i+1]!='.')
		return NOTANUMBER;			// первый ноль и за ним не точка

	TNumberType result=INTNUMBER;
	bool allZeroes=true;
	for( ; i<_input.size(); ++i)
	{
		if((_input[i]<'0' || _input[i]>'9') && _input[i]!='.')
			return NOTANUMBER;		// допускаюются только цифры и точка
		else if(_input[i]=='.' && result==INTNUMBER)
		{
			result=FLOATNUMBER;		// первая точка
			allZeroes=true;			
			continue;
		}
		else if(_input[i]=='.' && result==FLOATNUMBER)
			return NOTANUMBER;		// вторая точка

		if(_input[i]!='0' && _input[i]!='.')
			allZeroes=false;		
		else if(result==FLOATNUMBER)
			allZeroes=true;			// float не должен заканчиваться на 0
	}
	if(allZeroes && _input.size()>1)
		return NOTANUMBER;			// -0 тоже не число

	return result;
}
	
int CForthParser::Convert2Int(std::string& _input)
{
	assert(_input.size()>0);
	assert(_input[0]!='0' || _input.size()==1);
	assert(CheckNumber(_input)==INTNUMBER);

	int result=0;

	int sign=1;
	unsigned int i=0;
	if(_input[i]=='-')
	{
		sign=-1;
		++i;
	}

	for( ; i<_input.size(); ++i)
	{
		assert(_input[i]>='0' && _input[i]<='9');
		result*=10;
		int addition=_input[i]-'0';
		result+=addition;
	}

	return result*sign;	
}

TNumberType CForthParser::CheckNumber(char* _str)
{
	std::string input;
	input.assign(_str);
	return CheckNumber(input);	
}

int CForthParser::Convert2Int(char* _str)
{
	std::string input;
	input.assign(_str);
	return Convert2Int(input);	
}

CForthParser::~CForthParser()
{

}
