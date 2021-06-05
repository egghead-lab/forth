// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H)
#define AFX_STDAFX_H

#ifndef ECLIPSE							// MSVC 6.0
#pragma warning (disable : 4786)
#pragma warning (disable : 4172)
#pragma warning (disable : 4503)
#pragma warning (disable : 4996)
#include <windows.h>

#elif defined UNIX						// eclipse - unix
#include <unistd.h>

#else
#include <windows.h>					// eclipse - windows
#endif

#endif // !defined(AFX_STDAFX_H)
