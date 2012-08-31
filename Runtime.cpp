// ============================================================================
//
// This file contains routines that are handled during the Runtime
// 
// ============================================================================

// Common Include
#include	"Common.h"

string SubStr(const string& Str, const unsigned long& At, const unsigned long& Len);
string Escape(const string& Str, const string& To, const string& Esc);

vector<string> Split(const string& To, const string& By, const string& Esc, bool InvalEsc = false)
{
	vector<string> ret;
	string Item;
	for(unsigned long i = 0; i < To.length(); ++i)
	{
		if(SubStr(To, i, By.length()) == By)
		{
			ret.push_back(Item);
			Item = "";
			i += By.length() - 1;
		}
		else if(SubStr(To, i, MAX(Esc.length(), 1)) == Esc)
		{
			if(SubStr(To, i+MAX(Esc.length(), 1), By.length()) == By)
			{
				Item += By;
				i += MAX(Esc.length(), 1) + By.length() - 1;
			}
			else if(SubStr(To, i+MAX(Esc.length(), 1), MAX(Esc.length(), 1)) == Esc)
			{
				Item += Esc;
				i += MAX(Esc.length(), 1) + MAX(Esc.length(), 1) - 1;
			}
			else if(InvalEsc)
			{
				Item += Esc;
				i += MAX(Esc.length(), 1) - 1;
			}
		}
		else
		{
			Item += To.at(i);
		}
	}
	ret.push_back(Item);
	return(ret);
}

string GenerateLists(LPRDATA rdPtr, bool Linked) //Also used in SaveRunObject and LoadRunObject
{
	string ret;
	if(!Linked) //Contiguous
	{
		for(map<string, vector<string> >::iterator it = rdPtr->Lists.begin(); it != rdPtr->Lists.end(); ++it)
		{
			if(it != rdPtr->Lists.begin()){ ret += ';'; }
			ret += Escape(Escape(it->first, ",", "\\"), ";", "\\") + ';';
			for(unsigned long i = 0; i < it->second.size(); ++i)
			{
				if(i != 0){ ret += ','; }
				ret += Escape(Escape(it->second[i], ",", "\\"), ";", "\\");
			}
		}
	}
	else //Linked
	{
		for(map<string, list<string> >::iterator it = rdPtr->LLists.begin(); it != rdPtr->LLists.end(); ++it)
		{
			if(it != rdPtr->LLists.begin()){ ret += ';'; }
			ret += Escape(Escape(it->first, ",", "\\"), ";", "\\") + ';';
			for(list<string>::iterator jt = it->second.begin(); jt != it->second.end(); ++jt)
			{
				if(jt != it->second.begin()){ ret += ','; }
				ret += Escape(Escape(*jt, ",", "\\"), ";", "\\");
			}
		}
	}
	return(ret);
}

// --------------------
// GetRunObjectDataSize
// --------------------
// Returns the size of the runtime datazone of the object
// 
short WINAPI DLLExport GetRunObjectDataSize(fprh rhPtr, LPEDATA edPtr)
{
	return(sizeof(RUNDATA));
}


// ---------------
// CreateRunObject
// ---------------
// The routine where the object is actually created
// 
short WINAPI DLLExport CreateRunObject(LPRDATA rdPtr, LPEDATA edPtr, fpcob cobPtr)
{
	new (rdPtr) RUNDATA;
	// Do some rSDK stuff
	#include "rCreateRunObject.h"
	
	/*
	   This routine runs when your object is created, as you might have guessed.
	   It is here that you must transfer any data you need in rdPtr from edPtr,
	   because after this has finished you cannot access it again!
	   Also, if you have anything to initialise (e.g. dynamic arrays, surface objects)
	   you should do it here, and free your resources in DestroyRunObject.
	   See Graphic_Object_Ex.txt for an example of what you may put here.
	*/

	EditData ed;
	ed.Deserialize(edPtr);
	string &Lists = ed.Lists;
	string &LLists = ed.LLists;
	string &ListDelim = ed.ListDelim;
	string &ItemDelim = ed.ItemDelim;
	string &EscapeStr = ed.Esc;

	if(ItemDelim.empty()){ return(0); }

	if(!Lists.empty())
	{
		if(!ListDelim.empty())
		{
			vector<string> lists = Split(Lists, ListDelim, EscapeStr, true);
			for(unsigned long i = 0; i + 1 < lists.size(); ++ ++i)
			{
				rdPtr->CurrentList = lists[i];
				rdPtr->List() = Split(lists[i+1], ItemDelim, EscapeStr);
			}
			rdPtr->CurrentList = "";
		}
		else
		{
			rdPtr->List() = Split(Lists, ItemDelim, EscapeStr);
		}
	}
	if(!LLists.empty())
	{
		if(!ListDelim.empty())
		{
			vector<string> lists = Split(LLists, ListDelim, EscapeStr, true);
			for(unsigned long i = 0; i + 1 < lists.size(); ++ ++i)
			{
				rdPtr->CurrentLList = lists[i];
				vector<string> t = Split(lists[i+1], ItemDelim, EscapeStr);
				for(unsigned long j = 0; j < t.size(); ++j)
				{
					rdPtr->LList().push_back(t[j]);
				}
			}
			rdPtr->CurrentLList = "";
		}
		else
		{
			vector<string> t = Split(LLists, ItemDelim, EscapeStr);
			for(unsigned long i = 0; i < t.size(); ++i)
			{
				rdPtr->LList().push_back(t[i]);
			}
		}
	}

	// No errors
	return 0;
}

short WINAPI DLLExport DestroyRunObject(LPRDATA rdPtr, long fast)
{
	delete rdPtr->rRd;
	rdPtr->~tagRDATA();
	return 0;
}

short WINAPI DLLExport HandleRunObject(LPRDATA rdPtr)
{
	return REFLAG_ONESHOT;
}

short WINAPI DLLExport DisplayRunObject(LPRDATA rdPtr)
{
	return 0;
}

short WINAPI DLLExport PauseRunObject(LPRDATA rdPtr)
{
	return 0;
}

short WINAPI DLLExport ContinueRunObject(LPRDATA rdPtr)
{
	return 0;
}


// ============================================================================
//
// START APP / END APP / START FRAME / END FRAME routines
// 
// ============================================================================

// -------------------
// StartApp
// -------------------
// Called when the application starts or restarts.
// Useful for storing global data
// 
void WINAPI DLLExport StartApp(mv _far *mV, CRunApp* pApp)
{
	// Example
	// -------
	// Delete global data (if restarts application)
//	CMyData* pData = (CMyData*)mV->mvGetExtUserData(pApp, hInstLib);
//	if ( pData != NULL )
//	{
//		delete pData;
//		mV->mvSetExtUserData(pApp, hInstLib, NULL);
//	}
}

// -------------------
// EndApp
// -------------------
// Called when the application ends.
// 
void WINAPI DLLExport EndApp(mv _far *mV, CRunApp* pApp)
{
	// Example
	// -------
	// Delete global data
//	CMyData* pData = (CMyData*)mV->mvGetExtUserData(pApp, hInstLib);
//	if ( pData != NULL )
//	{
//		delete pData;
//		mV->mvSetExtUserData(pApp, hInstLib, NULL);
//	}
}

// -------------------
// StartFrame
// -------------------
// Called when the frame starts or restarts.
// 
void WINAPI DLLExport StartFrame(mv _far *mV, DWORD dwReserved, int nFrameIndex)
{

}

// -------------------
// EndFrame
// -------------------
// Called when the frame ends.
// 
void WINAPI DLLExport EndFrame(mv _far *mV, DWORD dwReserved, int nFrameIndex)
{

}

// ============================================================================
//
// DEBUGGER ROUTINES
// 
// ============================================================================

// --------------------
// Debugger
// --------------------

DEBUGGER_IDS_START()

	// DB_CURRENTSTRING,
	DB_EditList,

DEBUGGER_IDS_END()

DEBUGGER_ITEMS_START()

	//  DB_CURRENTSTRING,
	//	DB_CURRENTSTRING|DB_EDITABLE,
	//	DB_CURRENTVALUE|DB_EDITABLE,
	//	DB_CURRENTCHECK,
	//	DB_CURRENTCOMBO,
	DB_EditList|DB_EDITABLE,

DEBUGGER_ITEMS_END()

// -----------------
// GetDebugTree
// -----------------
// This routine returns the address of the debugger tree
//
LPWORD WINAPI DLLExport GetDebugTree(LPRDATA rdPtr)
{
#if !defined(RUN_ONLY)
	return DebugTree;
#else
	return NULL;
#endif // !defined(RUN_ONLY)
}

// -----------------
// GetDebugItem
// -----------------
// This routine returns the text of a given item.
//
void WINAPI DLLExport GetDebugItem(LPSTR pBuffer, LPRDATA rdPtr, int id)
{
#if !defined(RUN_ONLY)

	// Example
	// -------

//	char temp[DB_BUFFERSIZE];

	switch (id)
	{
/*	case DB_CURRENTSTRING:
		LoadString(hInstLib, IDS_CURRENTSTRING, temp, DB_BUFFERSIZE);
		wsprintf(pBuffer, temp, rdPtr->text);
		break;
	case DB_CURRENTVALUE:
		LoadString(hInstLib, IDS_CURRENTVALUE, temp, DB_BUFFERSIZE);
		wsprintf(pBuffer, temp, rdPtr->value);
		break;
	case DB_CURRENTCHECK:
		LoadString(hInstLib, IDS_CURRENTCHECK, temp, DB_BUFFERSIZE);
		if (rdPtr->check)
			wsprintf(pBuffer, temp, "TRUE");
		else
			wsprintf(pBuffer, temp, "FALSE");
		break;
	case DB_CURRENTCOMBO:
		LoadString(hInstLib, IDS_CURRENTCOMBO, temp, DB_BUFFERSIZE);
		wsprintf(pBuffer, temp, rdPtr->combo);
		break;*/
	case DB_EditList:
		strcpy(pBuffer, "Double-click to Edit/View");
		break;
	}


#endif // !defined(RUN_ONLY)
}

// -----------------
// EditDebugItem
// -----------------
// This routine allows to edit editable items.
//
void WINAPI DLLExport EditDebugItem(LPRDATA rdPtr, int id)
{
#if !defined(RUN_ONLY)

	// Example
	// -------

	switch (id)
	{
/*	case DB_CURRENTSTRING:
		{
			EditDebugInfo dbi;
			char buffer[256];

			dbi.pText=buffer;
			dbi.lText=TEXT_MAX;
			dbi.pTitle=NULL;

			strcpy(buffer, rdPtr->text);
			long ret=callRunTimeFunction(rdPtr, RFUNCTION_EDITTEXT, 0, (LPARAM)&dbi);
			if (ret)
				strcpy(rdPtr->text, dbi.pText);
		}
		break;
	case DB_CURRENTVALUE:
		{
			EditDebugInfo dbi;

			dbi.value=rdPtr->value;
			dbi.pTitle=NULL;

			long ret=callRunTimeFunction(rdPtr, RFUNCTION_EDITINT, 0, (LPARAM)&dbi);
			if (ret)
				rdPtr->value=dbi.value;
		}
		break;*/
	case DB_EditList:
		MessageBox(NULL, "In the future, I will let you edit this data, but for now it is view-only. Click OK to see the lists.", "Internal List Object", MB_OK);
		MessageBox(NULL, GenerateLists(rdPtr, false).c_str(), "Internal List Object - Contiguous Lists", MB_OK);
		MessageBox(NULL, GenerateLists(rdPtr, true).c_str(), "Internal List Object - Linked Lists", MB_OK);
		break;
	}

#endif // !defined(RUN_ONLY)
}

BOOL WINAPI SaveRunObject(LPRDATA rdPtr, HANDLE hFile)
{
#ifndef VITALIZE
	unsigned long Version = 3;
	string ToWrite = rdPtr->CurrentList + ';' + GenerateLists(rdPtr, false) + "\1" + rdPtr->CurrentLList + ';' + GenerateLists(rdPtr, true);
	unsigned long Written;
	if((WriteFile(hFile, &Version, sizeof(Version), &Written, NULL) == 0) && (WriteFile(hFile, ToWrite.c_str(), ToWrite.size() + 1, &Written, NULL) == 0))
	{
		return(FALSE);
	}
	return(TRUE);
#endif
	return(FALSE);
}

BOOL WINAPI LoadRunObject(LPRDATA rdPtr, HANDLE hFile)
{
	unsigned long Version;
	string Data;
	char *ch = new char[1];
	unsigned long Read;
	BOOL Success = TRUE;
	if((ReadFile(hFile, &Version, sizeof(Version), &Read, NULL) == 0) || (Version != 3))
	{
		return(FALSE);
	}
	while((Success = ReadFile(hFile, ch, 1, &Read, NULL)) != 0 && (*ch) != NULL)
	{
		Data += ch;
	}
	delete[] ch;

	string ListDelim = ";";
	string ItemDelim = ",";
	string EscapeStr = "\\";

	rdPtr->Lists.clear();
	rdPtr->LLists.clear();

	vector<string> CandL = Split(Data, "\1", "");
	vector<string> lists = Split(CandL[0], ListDelim, EscapeStr, true);
	string ListOn = lists.front();
	lists.erase(lists.begin());
	for(unsigned long i = 0; i + 1 < lists.size(); ++ ++i)
	{
		rdPtr->CurrentList = lists[i];
		rdPtr->List() = Split(lists[i+1], ItemDelim, EscapeStr);
	}
	rdPtr->CurrentList = ListOn;

	vector<string> Llists = Split(CandL[1], ListDelim, EscapeStr, true);
	string LListOn = Llists.front();
	Llists.erase(Llists.begin());
	for(unsigned long i = 0; i + 1 < Llists.size(); ++ ++i)
	{
		rdPtr->CurrentLList = Llists[i];
		vector<string> t = Split(Llists[i+1], ItemDelim, EscapeStr);
		for(unsigned long j = 0; j < t.size(); ++j)
		{
			rdPtr->LList().push_back(t[j]);
		}
	}
	rdPtr->CurrentLList = LListOn;

	return(Success);
}
