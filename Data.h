class rRundata;
typedef rRundata * LPRRDATA;

// --------------------------------
// RUNNING OBJECT DATA STRUCTURE
// --------------------------------
// If you want to store anything between actions/conditions/expressions
// you should store it here

typedef struct tagRDATA
{
	#include "MagicRDATA.h"

	map<string, vector<string> > Lists; //Contiguous Lists
	map<string, list<string> > LLists; //Linked Lists
	map<string, map<string, list<string>::iterator> > LLits; //Iterators for each Linked List
	string CurrentList; //Current Contiguous List
	string CurrentLList; //Current Linked List
	string CurrentLLit; //Current Iterator
	string CSa; //Custom Sort Item A
	string CSb; //Custom Sort Item B
	bool CSr; //Custom Sort Result
	tagRDATA(){}
	vector<string>& List()
	{
		return(Lists[CurrentList]);
	}
	list<string>& LList()
	{
		return(LLists[CurrentLList]);
	}
	list<string>::iterator& LLit()
	{
		if(LLits[CurrentLList].find(CurrentLLit) == LLits[CurrentLList].end())
		{
			LLits[CurrentLList][CurrentLLit] = LLists[CurrentLList].begin();
		}
		return(LLits[CurrentLList][CurrentLLit]);
	}
	list<string>::iterator& LLit(const string &name)
	{
		if(LLits[CurrentLList].find(name) == LLits[CurrentLList].end())
		{
			LLits[CurrentLList][name] = LLists[CurrentLList].begin();
		}
		return(LLits[CurrentLList][name]);
	}
	~tagRDATA(){}

} RUNDATA;
typedef	RUNDATA	* LPRDATA;

// --------------------------------
// EDITION OF OBJECT DATA STRUCTURE
// --------------------------------
// These values let you store data in your extension that will be saved in the MFA
// You should use these with properties

typedef struct tagEDATA_V2 //version 2, with manual string handling ("the buggy nightmare!")
{	extHeader		eHeader;
	unsigned long LDOffset, IDOffset, ESOffset;
	char Data;
} EDv2;

typedef struct tagEDATA_V3 //version 3, with advanced EditData class
{
	extHeader		eHeader; //Remember to change the version number below for edit data structure changes!
	char Data;

} EDITDATA;
typedef EDITDATA * LPEDATA;

extern unsigned char LastMenu;

struct EditData
{
	string Lists, LLists, ListDelim, ItemDelim, Esc;
	unsigned char LastMenu;

	EditData() : LastMenu(0) {}
	EditData(const EditData &from) : Lists(from.Lists), LLists(from.LLists), ListDelim(from.ListDelim), ItemDelim(from.ItemDelim), Esc(from.Esc), LastMenu(from.LastMenu) {}
	EditData &operator=(const EditData &from)
	{
		Lists = from.Lists;
		LLists = from.LLists;
		ListDelim = from.ListDelim;
		ItemDelim = from.ItemDelim;
		Esc = from.Esc;
		LastMenu = from.LastMenu;
	}
	EditData(LPEDATA &edPtr, LPMV &mV) : LastMenu(0) //may be changed by Deserialize
	{
		if(edPtr->eHeader.extVersion == 1) //version before properties were added
		{
			edPtr->eHeader.extVersion = 3; //update
		}
		else if(edPtr->eHeader.extVersion == 2) //version w/ properties, before Jamie's request
		{
			EDv2 *edv2 = (EDv2*)edPtr;
			Lists = &edv2->Data;
			ListDelim = &edv2->Data + edv2->LDOffset;
			ItemDelim = &edv2->Data + edv2->IDOffset;
			Esc = &edv2->Data + edv2->ESOffset;

			edPtr->eHeader.extVersion = 3; //update
			Serialize(edPtr, mV);
		}
		else if(edPtr->eHeader.extVersion == 3)
		{
			Deserialize(edPtr);
		}
		else
		{
			MessageBox(NULL, "This MFA was saved with an unkown version of the Internal List Object. Properties will be reset.\nNote that some actions, conditions, or expressions may not be valid and can cause MMF2 to crash.", "Warning - Internal List Object", MB_OK);
		}
	}

	void Serialize(LPEDATA &edPtr, LPMV &mV)
	{
		if(edPtr->eHeader.extVersion != 3){ EditData(edPtr, mV); return; } //update if needed
		if(::LastMenu != 0)
		{
			LastMenu = ::LastMenu;
		}
		unsigned long size = Lists.size()+1 + LLists.size()+1 + ListDelim.size()+1 + ItemDelim.size()+1 + Esc.size()+1 + sizeof(LastMenu);
		LPEDATA t = (LPEDATA)mvReAllocEditData(mV, edPtr, sizeof(EDITDATA)+size);
		if(!t)
		{
//			MessageBox(NULL, "Fatal Error: Cannot allocate space for the edit data. Changes will be lost.", "Error - Internal List Object", MB_OK);
			return;
		}
		edPtr = t;
		char *p = &edPtr->Data;
		memcpy(p, Lists.c_str(), Lists.size()+1);			p += Lists.size()+1;
		memcpy(p, LLists.c_str(), LLists.size()+1);			p += LLists.size()+1;
		memcpy(p, ListDelim.c_str(), ListDelim.size()+1);	p += ListDelim.size()+1;
		memcpy(p, ItemDelim.c_str(), ItemDelim.size()+1);	p += ItemDelim.size()+1;
		memcpy(p, Esc.c_str(), Esc.size()+1);				p += Esc.size()+1;
		memcpy(p, &LastMenu, sizeof(LastMenu));				p += sizeof(LastMenu);
	}
private:
	static void PassNull(char *&p)
	{
		while(*p)
		{
			++p;
		}
		++p; //go to right after the null
	}
public:
	void Deserialize(LPEDATA &edPtr)
	{
		char *p = &edPtr->Data;
		Lists = p; PassNull(p);
		LLists = p; PassNull(p);
		ListDelim = p; PassNull(p);
		ItemDelim = p; PassNull(p);
		Esc = p; PassNull(p);
		LastMenu = *((unsigned char*)p);
	}
};
