// decl.h		: declarations of frequently used classes used in forth machine
//
// Authors		: (a)pavelmalyshkin
// License		: ELEPHANT
//

#if !defined(DECL_H)
#define DECL_H

typedef enum {
	Ok,
	WAIT,
	ABORT,
	QUIT,
	UNDEFINED,
	WRONG_LIST,
	WRONG_STACK,
	WRONG_VALUE,
	OVERFLOW
} TForthErrorCode;

typedef enum {
	INTNUMBER,
	FLOATNUMBER,
	NOTANUMBER
} TNumberType;

class CForthEnvironment;
class CForthTreeNode;
class CForthMachine;
class CCompileData;
class CDefinition;

typedef TForthErrorCode (*TWordFunction)(CForthMachine*, int);

#endif // !defined(DECL_H)
