// Num2Str.cpp	: implementation of the Num2Str function.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Num2Str.h"

std::string Num2String(int _num)
{
	std::string str;
	if(_num==0)
		str+="0 ";
	else
	{
		if(_num<0)
		{
			str+='-';
			_num=-_num;
		}
		std::string mstr;
		for( ; _num>0; _num/=10)
		{
			int div=_num-_num/10*10;
			mstr=char(48+div)+mstr;
		}
		str+=mstr+' ';
	}
	return str;
}