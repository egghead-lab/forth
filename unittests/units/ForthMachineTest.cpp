// ForthMachineTest.cpp	: implementation of the CForthMachineTest class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ForthMachineTest.h"
#include "../../forth/ForthConsole.h"

#include <string>
#include <iostream>
#include <assert.h>

#define TRY(in,wordS,stackS,s,out)	{								\
	machine.Do(in);													\
	machine.Run();													\
	assert(machine.GetWordNumber()==wordS);							\
	assert(machine.GetStackSize()==stackS);							\
	assert(machine.GetStatus()==s);									\
	std::string stString=out;										\
	std::vector<std::string> stNums=parser.Parse(stString);			\
	for(unsigned int i=0; i<stackS; ++i)							\
	assert(parser.Convert2Int(stNums[i])==machine.GetStackItem(i));	}

//////////////////////////////////////////////////////////////////////
// class CForthMachineTest
//////////////////////////////////////////////////////////////////////

CForthMachineTest::CForthMachineTest()
{

}

void CForthMachineTest::Run(bool _echo)
{
	std::cout << " Forth machine test";
	bool result=true;

	{
		// Parsing numbers
		CForthParser parser;
		for(int i=-20; i<=20; ++i)
		{
			std::string number;
			if(i<0)
				number.assign("-");
			else
				number.assign("");
			
			int j=i<0?-i:i;
			if(j/10>0)
				number+='0'+j/10;
			number+='0'+j%10;

			assert(parser.CheckNumber(number)==INTNUMBER);
			assert(parser.Convert2Int(number)==i);
		}

		assert(parser.CheckNumber("-0")==NOTANUMBER);
		assert(parser.CheckNumber("-00")==NOTANUMBER);
		assert(parser.CheckNumber("00")==NOTANUMBER);
		assert(parser.CheckNumber("-1.0")==NOTANUMBER);
		assert(parser.CheckNumber(".0")==NOTANUMBER);
		assert(parser.CheckNumber("0.0")==NOTANUMBER);
		assert(parser.CheckNumber("0.")==NOTANUMBER);
		assert(parser.CheckNumber("-.0")==NOTANUMBER);
		assert(parser.CheckNumber("-0.0")==NOTANUMBER);
		assert(parser.CheckNumber("-0.")==NOTANUMBER);
	}

	CForthParser parser;
	CForthMachine machine(_echo);
	const unsigned int iwn=machine.GetWordNumber();
	assert(machine.GetStatus()==Ok);

	{
		// integer numbers
		for(int i=-20; i<=20; ++i)
		{
			std::string number;
			if(i<0)
				number.assign("-");
			else
				number.assign("");
			
			int j=i<0?-i:i;
			if(j/10>0)
				number+='0'+j/10;
			number+='0'+j%10;

			machine.Do(number);
			machine.Run();
			assert(machine.GetStackSize()>0);
			assert(machine.GetStackTop()==i);
			assert(machine.GetStatus()==Ok);
		}

		assert(machine.GetStackSize()==41);
		machine.Do("ABORT");
		machine.Run();
		assert(machine.GetStackSize()==0);
		assert(machine.GetStatus()==Ok);
	}

	{
		// 10 20 30 40
		// THEN ELSE IF
		// VARIABLE
		// CONSTANT
		// ;
		// @ !
		TRY("RESET",iwn,0,Ok,"");
		TRY(" 10",iwn,1,Ok,"10");
		TRY("20 ",iwn,2,Ok,"10 20");
		TRY(" 30  40 ",iwn,4,Ok,"10 20 30 40");
		TRY("THEN",iwn,0,WRONG_LIST,"");
		TRY("ELSE",iwn,0,WRONG_LIST,"");
		TRY("IF",iwn,0,WRONG_STACK,"");
		TRY("0 IF  ELSE  THEN",iwn,0,Ok,"");
		TRY("0 IF",iwn,0,WAIT,"");
		TRY("ELSE",iwn,0,WAIT,"");
		TRY("THEN",iwn,0,Ok,"");
		TRY("VARIABLE",iwn,0,WAIT,"");
		TRY("NEW",iwn+1,0,Ok,"");
		TRY("FORGET",iwn+1,0,WAIT,"");
		TRY("NEW",iwn,0,Ok,"");
		TRY(" CONSTANT",iwn,0,WRONG_STACK,"");
		TRY("0   CONSTANT",iwn,1,WAIT,"0");
		TRY("NEW ",iwn+1,0,Ok,"");
		TRY("FORGET NEW :",iwn,0,WAIT,"");
		TRY(";",iwn,0,WAIT,"");
		TRY(";",iwn+1,0,Ok,"");
		TRY("FORGET ;",iwn,0,Ok,"");
		TRY(";",iwn,0,WRONG_LIST,"");
		TRY("FORGET ABRAKADABRA",iwn,0,WRONG_LIST,"");
		TRY("@",iwn,0,WRONG_STACK,"");
		TRY("0 @",iwn,0,WRONG_VALUE,"");
		TRY("5 !",iwn,0,WRONG_VALUE,"");
		TRY("VARIABLE NEW NEW !",iwn+1,0,WRONG_STACK,"");
		TRY("RESET ABRAKADABRA",iwn,0,UNDEFINED,"");
	}

	{
		// 10 CONSTANT 9 9 => ( 10 )
		// RESET 9 => ( 9 )
		// VARIABLE 9 10 9 ! => ( 9 )
		// 9 @ => ( 9 10 )
		// ABORT FORGET 9 9 => 9
		TRY("RESET 10 CONSTANT 9 9",iwn+1,1,Ok,"10");
		TRY("RESET 9",iwn,1,Ok,"9");
		TRY("VARIABLE 9 10 9 !",iwn+1,1,Ok,"9");
		TRY("9 @",iwn+1,2,Ok,"9 10");
		TRY("ABORT",iwn+1,0,Ok,"");
		TRY("FORGET 9",iwn,0,Ok,"");
		TRY("9",iwn,1,Ok,"9");
		TRY("ABORT",iwn,0,Ok,"");
	}

	{
		// : NEW 10 CONSTANT 9 9
		// ; NEW 9 => (10 9)
		TRY("RESET : NEW 10 CONSTANT 9 9",iwn,0,WAIT,"");
		TRY("; NEW 9",iwn+1,2,Ok,"10 9");
		TRY("FORGET NEW",iwn,2,Ok,"10 9");
		TRY("FORGET 9",iwn,0,WRONG_LIST,"");
	}

	{
		// : NEW FORGET ;
		// : NEW NEW NEW ;
		// NEW
		// NEW NEW
		TRY("RESET : NEW FORGET ;",iwn+1,0,Ok,"");
		TRY(": NEW NEW NEW ;",iwn+2,0,Ok,"");
		TRY("NEW",iwn+1,0,Ok,"");
		TRY("NEW NEW",iwn,0,Ok,"");
	}

	{
		// : NEW 1 2 3 ;
		// NEW
		TRY("RESET : NEW 1 2 3 ;",iwn+1,0,Ok,"");
		TRY("NEW",iwn+1,3,Ok,"1 2 3");
		TRY("FORGET NEW ABORT",iwn,0,Ok,"");
	}

	{
		// 0 IF 10 ELSE 20 THEN 30 => ( 20 30 )
		// ABORT 1 IF 10 ELSE 20 THEN 30 => ( 10 30 )
		TRY("RESET 0 IF 10 ELSE 20 THEN 30",iwn,2,Ok,"20 30");
		TRY("ABORT 1 IF 10 ELSE 20 THEN 30",iwn,2,Ok,"10 30");
		TRY("ABORT",iwn,0,Ok,"");
	}

	{
		// 0 IF ELSE 20 THEN => (20)
		// IF 30 ELSE THEN => (30)
		// ABORT 0 IF 10 ELSE THEN => ()
		// 1 IF ELSE 20 THEN => ()
		TRY("RESET 0 IF ELSE 20 THEN",iwn,1,Ok,"20");
		TRY("IF 30 ELSE THEN",iwn,1,Ok,"30");
		TRY("ABORT 0 IF 10 ELSE THEN",iwn,0,Ok,"");
		TRY("1 IF ELSE 20 THEN",iwn,0,Ok,"");
	}

	{
		// : NEW IF 10 ELSE 20 THEN 30 ;
		// 1 NEW => ( 10 30 )
		// ABORT 0 NEW => ( 20 30 )
		TRY("RESET : NEW IF 10 ELSE 20 THEN 30 ;",iwn+1,0,Ok,"");
		TRY("1 NEW",iwn+1,2,Ok,"10 30");
		TRY("ABORT 0 NEW",iwn+1,2,Ok,"20 30");
		TRY("FORGET NEW ABORT",iwn,0,Ok,"");
	}

	{
		// : NEW IF ELSE 20 THEN ; 0 NEW => (20)
		// NEW => ()
		// : NEW IF 30 ELSE THEN ;
		// 0 NEW => ()
		// 1 NEW => (30)
		TRY("RESET : NEW IF ELSE 20 THEN ; 0 NEW",iwn+1,1,Ok,"20");
		TRY("NEW",iwn+1,0,Ok,"");
		TRY(": NEW IF 30 ELSE THEN ; 0 NEW",iwn+2,0,Ok,"");
		TRY("1 NEW",iwn+2,1,Ok,"30");
		TRY("FORGET NEW FORGET NEW ABORT",iwn,0,Ok,"");
	}

	{
		// : NEW IF VARIABLE 9 10 9 ! ELSE 10 CONSTANT 9 9 ;
		// 0 NEW => ( 10 )
		// THEN 9 FORGET 9 NEW THEN 9 @ => 
		TRY("RESET : NEW IF VARIABLE 9 20 9 ! 9 @ ELSE 10 CONSTANT 9 9 ;",iwn+1,0,Ok,"");
		TRY("0 NEW",iwn+1,1,WAIT,"10");
		TRY("THEN 9",iwn+1,2,Ok,"10 9"); 
		TRY("FORGET 9",iwn+1,0,WRONG_LIST,"");
		TRY("1 NEW THEN 9",iwn+1,2,Ok,"20 9");
	}

	{
		// : NEW IF 20 ELSE 30 ; 0 NEW THEN => (30)
		// NEW THEN => (20)
		// : NEW IF 30 ; NEW THEN => (30)
		// ABORT 0 NEW THEN => ()
		// : NEW IF ELSE 10 ; 0 NEW THEN => (10)
		TRY("RESET : NEW IF 20 ELSE 30 ; 0 NEW THEN",iwn+1,1,Ok,"30");
		TRY("NEW",iwn+1,1,WAIT,"20");
		TRY("THEN",iwn+1,1,Ok,"20");
		TRY(": NEW IF 30 ; NEW THEN",iwn+2,1,Ok,"30");
		TRY("ABORT 0 NEW THEN",iwn+2,0,Ok,"");
		TRY(": NEW IF ELSE 10 ; 0 NEW THEN",iwn+3,1,Ok,"10");
	}

	{
		// : NEW IF 20 ; 0 NEW 30 THEN => ()
		// 1 NEW 30 THEN => ( 20 30 )
		// : NEW IF ELSE 10 ; NEW 40 THEN => (20)
		// NEW NEW THEN => ()
		// 0 NEW 40 THEN => ( 10 40 )
		// FORGET NEW NEW NEW THEN 30 THEN => ( 10 20 30 )
		TRY("RESET : NEW IF 20 ; 0 NEW 30 THEN",iwn+1,0,Ok,"");
		TRY("1 NEW 30 THEN",iwn+1,2,Ok,"20 30");
		TRY(": NEW IF ELSE 10 ; NEW 40 THEN",iwn+2,1,Ok,"20");
		TRY("NEW NEW THEN",iwn+2,0,Ok,"");
		TRY("0 NEW 40 THEN",iwn+2,2,Ok,"10 40");
		TRY("FORGET NEW NEW NEW THEN",iwn+1,2,WAIT,"10 20");
		TRY("30 THEN",iwn+1,3,Ok,"10 20 30");
	}

	{
		// : NEW ; NEW
		// FORGET
		// NEW
		TRY("RESET :",iwn,0,WAIT,"");
		TRY("NEW ; NEW",iwn+1,0,Ok,"");
		TRY("FORGET",iwn+1,0,WAIT,"");
		TRY("NEW",iwn,0,Ok,"");
	}

	{
		// VARIABLE TEN 10 TEN !
		// : NEW TEN ; NEW @ => (10)
		// TEN TEN => ( 10 0 0 )
		// FORGET TEN @ => ( 10 0 10 )
		// IF THEN NEW @ => ( 10 0 10 )
		// FORGET NEW IF THEN @ 
		TRY("RESET VARIABLE TEN 10 TEN !",iwn+1,0,Ok,"");
		TRY(": NEW TEN ; NEW @",iwn+2,1,Ok,"10");
		TRY("TEN TEN",iwn+2,3,Ok,"10 0 0");
		TRY("FORGET TEN @",iwn+1,3,Ok,"10 0 10");
		TRY("IF THEN NEW @",iwn+1,3,Ok,"10 0 10");
		TRY("FORGET NEW IF THEN @",iwn,2,Ok,"10 10");
	}

	{
		// ABRAKADABRA
		// : ABRAKADABRA 10 ;
		// ABRAKADABRA => (10)
		// : NEW ABRAKADABRA ;
		// FORGET ABRAKADABRA ABRAKADABRA
		// NEW
		TRY("RESET ABRAKADABRA",iwn,0,UNDEFINED,"");
		TRY(": ABRAKADABRA 10 ;",iwn+1,0,Ok,"");
		TRY("ABRAKADABRA",iwn+1,1,Ok,"10");
		TRY(": NEW ABRAKADABRA ;",iwn+2,1,Ok,"10");
		TRY("FORGET ABRAKADABRA ABRAKADABRA",iwn+1,0,UNDEFINED,"");
		TRY("NEW",iwn+1,1,Ok,"10");
	}

	{
		// : NEW ABRAKADABRA ;
		// NEW => ()
		// : ABRAKADABRA 10 ;
		// ABRAKADABRA => (10)
		// NEW => ()
		// FORGET ABRAKADABRA NEW => ()
		TRY("RESET : NEW ABRAKADABRA ;",iwn+1,0,Ok,"");
		TRY("NEW",iwn+1,0,WRONG_LIST,"");
		TRY(": ABRAKADABRA 10 ;",iwn+2,0,Ok,"");
		TRY("ABRAKADABRA",iwn+2,1,Ok,"10");
		TRY("NEW",iwn+2,0,WRONG_LIST,"");
		TRY("FORGET ABRAKADABRA NEW",iwn+1,0,WRONG_LIST,"");
	}

	{
		// : if IF ;
		// : else ELSE ; 
		// : then THEN ;
		// 0 if else 10 then if 20 then => (20)
		// : NEW 0 if else 10 then if 20 then ; NEW 
		TRY("RESET : if IF ;", iwn+1,0,Ok,"");
		TRY(": else ELSE ;",iwn+2,0,Ok,"");
		TRY(": then THEN ;",iwn+3,0,Ok,"");
		TRY("0 if else 10 then if 20 else then",iwn+3,1,Ok,"20");
		TRY("if then : NEW 0 if else 10 then if 30 then ; NEW",iwn+4,1,Ok,"30");
	}

	{
		// BEGIN 1 UNTIL
		// BEGIN 0 UNTIL
		// ABORT
		TRY("RESET BEGIN 1 UNTIL",iwn,0,Ok,"");

		CBreakingEnvironment env;
		machine.SetEnvironment(&env);
		TRY("BEGIN 0 UNTIL",iwn,0,ABORT,"");
		assert(env.GetCounter()==env.GetThreshold());
	}

	{
		// : NEW IF 5 ELSE 10 NEW THEN ;
		// 0 NEW => 5
		TRY("RESET : NEW IF 5 ELSE 10 NEW THEN ;",iwn+1,0,Ok,"");

		machine.SetEnvironment(NULL);
		TRY("0 NEW",iwn+1,1,Ok,"5");
	}

	{
		// : NEW BEGIN 1 ;
		// NEW UNTIL
		// FORGET NEW : NEW 1 UNTIL ;
		// BEGIN NEW
		TRY("RESET : NEW BEGIN 1 ;",iwn+1,0,Ok,"");
		TRY("NEW UNTIL",iwn+1,0,Ok,"");
		TRY("FORGET NEW : NEW 1 UNTIL ;",iwn+1,0,Ok,"");
		TRY("BEGIN NEW",iwn+1,0,Ok,"");
		assert(machine.GetMaxLevel()==1);
	}

	{
		// : NEW BEGIN 0 ;
		// NEW UNTIL
		TRY("RESET : NEW BEGIN 0 ;",iwn+1,0,Ok,"");

		CBreakingEnvironment env;
		machine.SetEnvironment(&env);
		TRY("NEW UNTIL",iwn+1,0,ABORT,"");
		assert(env.GetCounter()==env.GetThreshold());
		assert(machine.GetMaxLevel()==1);
	}

	{
		// : NEW 0 UNTIL ;
		// BEGIN NEW
		TRY("RESET : NEW 0 UNTIL ;",iwn+1,0,Ok,"");

		CBreakingEnvironment env;
		machine.SetEnvironment(&env);
		TRY("BEGIN NEW",iwn+1,0,ABORT,"");
		assert(env.GetCounter()==env.GetThreshold());
		assert(machine.GetMaxLevel()==1);
	}

	{
		// 1 BEGIN 1 BEGIN UNTIL UNTIL
		// 1 BEGIN BEGIN 0 UNTIL UNTIL
		// BEGIN 0 1 BEGIN UNTIL UNTIL
		// BEGIN 0 BEGIN 0 UNTIL UNTIL
		machine.SetEnvironment(NULL);
		TRY("RESET 1 BEGIN 1 BEGIN UNTIL UNTIL",iwn,0,Ok,"");
		assert(machine.GetMaxLevel()==0);

		CBreakingEnvironment env;
		machine.SetEnvironment(&env);
		TRY("1 BEGIN BEGIN 0 UNTIL UNTIL",iwn,0,ABORT,"");
		assert(env.GetCounter()==env.GetThreshold());
		assert(machine.GetMaxLevel()==0);

		env.Clear();
		TRY("BEGIN 0 1 BEGIN UNTIL UNTIL",iwn,0,ABORT,"");
		assert(env.GetCounter()==env.GetThreshold());
		assert(machine.GetMaxLevel()==0);

		env.Clear();
		TRY("BEGIN 0 BEGIN 0 UNTIL UNTIL",iwn,0,ABORT,"");
		assert(env.GetCounter()==env.GetThreshold());
		assert(machine.GetMaxLevel()==0);
	}

	{
		// : NEW 1 BEGIN UNTIL UNTIL;
		// 1 BEGIN NEW
		machine.SetEnvironment(NULL);
		TRY(": NEW 1 BEGIN UNTIL ;",iwn+1,0,Ok,"");
		TRY("1 BEGIN NEW UNTIL",iwn+1,0,Ok,"");

		// : NEW BEGIN 0 UNTIL ;
		// 1 BEGIN NEW
		TRY("FORGET NEW : NEW BEGIN 0 UNTIL ;",iwn+1,0,Ok,"");
		CBreakingEnvironment env;
		machine.SetEnvironment(&env);
		TRY("1 BEGIN NEW",iwn+1,0,ABORT,"");
		assert(env.GetCounter()==env.GetThreshold());
		assert(machine.GetMaxLevel()==1);
	}

	{
		// 1 IF BEGIN ELSE BEGIN THEN 1 1 IF UNTIL THEN
		// 1 IF BEGIN THEN 1 0 IF UNTIL ELSE UNTIL THEN
		machine.SetEnvironment(NULL);
		TRY("RESET 1 IF BEGIN ELSE BEGIN THEN 1 1 IF UNTIL THEN",iwn,0,WRONG_LIST,"");
		TRY("1 IF BEGIN THEN 1 0 IF UNTIL ELSE UNTIL THEN",iwn,0,WRONG_LIST,"");
	}

	{
		// : NEW BEGIN 1 UNTIL ;
		// BEGIN NEW 1 UNTIL ;
		// : ANOTHERNEW BEGIN NEW 1 UNTIL ;
		// BEGIN ANOTHERNEW NEW 1 UNTIL
		TRY("RESET : NEW BEGIN 1 UNTIL ;",iwn+1,0,Ok,"");
		TRY("BEGIN NEW 1 UNTIL",iwn+1,0,Ok,"");
		TRY(": ANOTHERNEW BEGIN NEW 1 UNTIL ;",iwn+2,0,Ok,"");
		TRY("BEGIN ANOTHERNEW NEW 1 UNTIL",iwn+2,0,Ok,"");
	}

	{
		// 1 BEGIN WHILE 20 0 REPEAT => (20)
		// 1 IF 1 BEGIN WHILE 30 0 REPEAT ELSE 1 BEGIN WHILE 40 0 REPEAT THEN => (30)
		// 0 IF 1 BEGIN WHILE 30 0 REPEAT ELSE 1 BEGIN WHILE 40 0 REPEAT THEN => (40)
		// 0 BEGIN IF 0 ELSE 1 THEN WHILE 50 1 REPEAT	=> (50)
		// 1 0 1 BEGIN WHILE IF 0 ELSE 1 THEN REPEAT
		TRY("RESET 1 BEGIN WHILE 20 0 REPEAT",iwn,1,Ok,"20");
		TRY("IF 1 BEGIN WHILE 30 0 REPEAT ELSE 1 BEGIN WHILE 40 0 REPEAT THEN",iwn,1,Ok,"30");
		TRY("ABORT 0 IF 1 BEGIN WHILE 30 0 REPEAT ELSE 1 BEGIN WHILE 40 0 REPEAT THEN",iwn,1,Ok,"40");
		TRY("ABORT 0 BEGIN IF 0 ELSE 1 THEN WHILE 50 1 REPEAT",iwn,1,Ok,"50");
		TRY("ABORT 1 0 1 BEGIN WHILE IF 0 ELSE 1 THEN REPEAT",iwn,0,Ok,"");
		TRY(": NEW BEGIN WHILE 20 0 REPEAT ; 1 NEW",iwn+1,1,Ok,"20");
		TRY(": NEW30 BEGIN WHILE 30 0 REPEAT ; : NEW40 1 BEGIN WHILE 40 0 REPEAT ; IF 1 NEW30 ELSE NEW40 THEN",iwn+3,1,Ok,"30");
		TRY("ABORT 0 IF 1 NEW30 ELSE NEW40 THEN",iwn+3,1,Ok,"40");
		TRY("RESET : NEW IF 0 ELSE 1 THEN ; 0 BEGIN NEW WHILE 50 1 REPEAT",iwn+1,1,Ok,"50");
		TRY("ABORT 1 0 1 BEGIN WHILE NEW REPEAT",iwn+1,0,Ok,"");

		TRY("RESET : NEW 0 ; NEW BEGIN IF 0 ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 0 BEGIN ; NEW IF 0 ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 0 BEGIN IF ; NEW 0 ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 0 BEGIN IF 0 ; NEW ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 0 BEGIN IF 0 ELSE ; NEW 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 0 BEGIN IF 0 ELSE 1 ; NEW THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 0 BEGIN IF 0 ELSE 1 THEN ; NEW WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 0 BEGIN IF 0 ELSE 1 THEN WHILE ; NEW 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 0 BEGIN IF 0 ELSE 1 THEN WHILE 50 ; NEW 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 0 BEGIN IF 0 ELSE 1 THEN WHILE 50 1 ; NEW REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 0 BEGIN IF 0 ELSE 1 THEN WHILE 50 1 REPEAT ; NEW", iwn+1,1,Ok,"50");

		TRY("RESET : NEW ; 0 NEW BEGIN IF 0 ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW BEGIN ; 0 NEW IF 0 ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW BEGIN IF ; 0 NEW 0 ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW BEGIN IF 0 ; 0 NEW ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW BEGIN IF 0 ELSE ; 0 NEW 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW BEGIN IF 0 ELSE 1 ; 0 NEW THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW BEGIN IF 0 ELSE 1 THEN ; 0 NEW WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW BEGIN IF 0 ELSE 1 THEN WHILE ; 0 NEW 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW BEGIN IF 0 ELSE 1 THEN WHILE 50 ; 0 NEW 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW BEGIN IF 0 ELSE 1 THEN WHILE 50 1 ; 0 NEW REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW BEGIN IF 0 ELSE 1 THEN WHILE 50 1 REPEAT ; 0 NEW", iwn+1,1,Ok,"50");

		TRY("RESET : NEW ; 0 BEGIN NEW IF 0 ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW IF ; 0 BEGIN NEW 0 ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW IF 0 ; 0 BEGIN NEW ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW IF 0 ELSE ; 0 BEGIN NEW 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW IF 0 ELSE 1 ; 0 BEGIN NEW THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW IF 0 ELSE 1 THEN ; 0 BEGIN NEW WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW IF 0 ELSE 1 THEN WHILE ; 0 BEGIN NEW 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW IF 0 ELSE 1 THEN WHILE 50 ; 0 BEGIN NEW 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW IF 0 ELSE 1 THEN WHILE 50 1 ; 0 BEGIN NEW REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW IF 0 ELSE 1 THEN WHILE 50 1 REPEAT ; 0 BEGIN NEW", iwn+1,1,Ok,"50");

		TRY("RESET : NEW ; 0 BEGIN IF NEW 0 ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 0 ; 0 BEGIN IF NEW ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");

		TRY("RESET : NEW ; 0 BEGIN IF 0 NEW ELSE 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW ELSE ; 0 BEGIN IF 0 NEW 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");

/*		TRY("RESET : NEW ; 0 BEGIN IF 0 ELSE NEW 1 THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 1 ; 0 BEGIN IF 0 ELSE NEW THEN WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 1 THEN ; 0 BEGIN IF 0 ELSE NEW WHILE 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 1 THEN WHILE ; 0 BEGIN IF 0 ELSE NEW 50 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 1 THEN WHILE 50 ; 0 BEGIN IF 0 ELSE NEW 1 REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 1 THEN WHILE 50 1 ; 0 BEGIN IF 0 ELSE NEW REPEAT", iwn+1,1,Ok,"50");
		TRY("RESET : NEW 1 THEN WHILE 50 1 REPEAT ; 0 BEGIN IF 0 ELSE NEW", iwn+1,1,Ok,"50");*/
}

	// EXIT
	TRY("RESET WORDS",iwn,0,Ok,"");
	TRY("QUIT",0,0,QUIT,"");

	FinalizeTest(result);
}

//////////////////////////////////////////////////////////////////////
// class CBreakingEnvironment
//////////////////////////////////////////////////////////////////////

CBreakingEnvironment::CBreakingEnvironment():myThreshold(10)
{
	Clear();
}

void CBreakingEnvironment::TakeControl(CForthMachine& _machine)
{
	if(myCallCounter<myThreshold)
	{
		std::string command="COMMAND";
		command+=char(48+myCallCounter);
		_machine.Do(command);
		++myCallCounter;
	}
	else
		_machine.Do("ABORT");
}

void CBreakingEnvironment::Clear()
{
	myCallCounter=0;
	myLastCode=Ok;
}