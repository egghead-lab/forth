// ForthParser.h: interface for the CForthParser class.
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//
//////////////////////////////////////////////////////////////////////

#if !defined(FORTHPARSER_H)
#define FORTHPARSER_H

#include "decl.h"
#include <vector>
#include <string>

//////////////////////////////////////////////////////////////////////
// class CForthParser
//////////////////////////////////////////////////////////////////////

class CForthParser  
{
	std::vector<std::string> myOutput;

public:
	CForthParser();

	std::vector<std::string>& Parse(std::string);
	TNumberType CheckNumber(std::string&);
	TNumberType CheckNumber(char*);
	int Convert2Int(std::string&);
	int Convert2Int(char*);

	virtual ~CForthParser();
};

#endif // !defined(FORTHPARSER_H)
