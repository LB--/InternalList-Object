// Include guard
#ifndef COMMON_H
#define COMMON_H

// Suppress the deprecated warnings for VC2005
#define _CRT_SECURE_NO_WARNINGS

// General includes
#include	"TemplateInc.h"

//#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <list>
//#include <utility>
#include <algorithm>
#include <fstream>
#include <regex>
using namespace std;
#include "StringConvert.hpp"

// Specific to this extension
#include	"Resource.h"
#include	"FlagsPrefs.h"
#include	"Information.h"
#include	"Data.h"

// rTemplate include
#include	"rTemplate.h"

// Globals and prototypes
extern HINSTANCE hInstLib;
extern short * conditionsInfos;
extern short * actionsInfos;
extern short * expressionsInfos;
extern long (WINAPI ** ConditionJumps)(LPRDATA rdPtr, long param1, long param2);
extern short (WINAPI ** ActionJumps)(LPRDATA rdPtr, long param1, long param2);
extern long (WINAPI ** ExpressionJumps)(LPRDATA rdPtr, long param);
extern PropData Properties[];
extern WORD DebugTree[];

class CSO //Custom Sort Callback Object
{
	CSO(){}
public:
	LPRDATA rdPtr;
	CSO(LPRDATA rdPtr) : rdPtr(rdPtr) {}
	CSO(const CSO& from) : rdPtr(from.rdPtr) {}
	bool operator()(const string& a, const string& b)
	{
		rdPtr->CSa = a;
		rdPtr->CSb = b;
		rdPtr->rRd->GenerateEvent(1);
		rdPtr->CSa = "";
		rdPtr->CSb = "";
		return(rdPtr->CSr);
	}
};

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef LIMIT
#define LIMIT(v,n,x) (MIN(x, MAX(n, v)))
#endif

//Exceptionally bad Exception Catching & Reporting
#define TRY try{
#define CATCH }catch(...){rdPtr->rRd->GenerateEvent(2);}

// End include guard
#endif

extern unsigned char LastMenu; //Edittime Only
