//Contiguous Lists
#include "Common.h"
#include "ParamHelp.h"

string SubStr(const string& Str, const unsigned long& At, const unsigned long& Len)
{
	string Temp;
	for(unsigned long i = At; i < MIN(At + Len, Str.length()); ++i)
	{
		Temp += Str.at(i);
	}
	return(Temp);
}

string Escape(const string& Str, const string& To, const string& Esc)
{
	string Temp;
	for(unsigned long i = 0; i < Str.length(); ++i)
	{
		if(SubStr(Str, i, To.length()) == To)
		{
			Temp += Esc + To;
			i += MAX(To.length(), 1) - 1;
		}
		else if(SubStr(Str, i, MAX(Esc.length(), 1)) == Esc)
		{
			Temp += Esc + Esc;
			i += MAX(Esc.length(), 1) - 1;
		}
		else
		{
			Temp += Str.at(i);
		}
	}
	return(Temp);
}
vector<string> Split(const string& To, const string& By, const string& Esc, bool InvalEsc = false);

bool ForwardSort(const string& a, const string& b)
{
/*	for(unsigned long i = 0; i < MIN(a.length(), b.length()); ++i)
	{
		if(a.at(i) > b.at(i))
		{
			return(false);
		}
		else if(a.at(i) < b.at(i))
		{
			return(true);
		}
	}
	if(a.length() > b.length()){ return(false); }
	return(true);
*/	return(a < b);
}

bool ReverseSort(const string& a, const string& b)
{
/*	for(unsigned long i = 0; i < MIN(a.length(), b.length()); ++i)
	{
		if(a.at(i) < b.at(i))
		{
			return(false);
		}
		else if(a.at(i) > b.at(i))
		{
			return(true);
		}
	}
	if(a.length() < b.length()){ return(false); }
	return(true);
*/	return(a > b);
}

#include "Alphanum.hpp"

bool ForwardAlphanumSort(const string& a, const string& b)
{
	return(doj::alphanum_comp(a, b) < 0 ? true : false);
}

bool ReverseAlphanumSort(const string& a, const string& b)
{
	return(doj::alphanum_comp(a, b) > 0 ? true : false);
}

// ============================================================================
//
// CONDITIONS === Contiguous Lists
// 
// ============================================================================

CONDITION(
	/* ID */			0,
	/* Name */			"%o: Item %0 is in List",
	/* Flags */			EVFLAGS_ALWAYS|EVFLAGS_NOTABLE,
	/* Params */		(1,PARAM_STRING,"Item String")
) {
	string Item = StrParam();

	for(unsigned long i = 0; i < rdPtr->List().size(); ++i)
	{
		if(Item == rdPtr->List().at(i))
		{
			return(true);
		}
	}
	return(false);
}

CONDITION(
	/* ID */			1,
	/* Name */			"%o: On Custom Sort",
	/* Flags */			0,
	/* Params */		(0)
) {
	return(true);
}

CONDITION(
	/* ID */			2,
	/* Name */			"%o: On Error",
	/* Flags */			0,
	/* Params */		(0)
) {
	return(true);
}

/* ******* ******* ******* Regular Expressions ******* ******* ******* */
CONDITION(
	/* ID */			3,
	/* Name */			"%o: Item matching %0 is in List",
	/* Flags */			EVFLAGS_ALWAYS|EVFLAGS_NOTABLE,
	/* Params */		(1,PARAM_STRING,"Regular Expression String")
) {
	tr1::regex RegEx (StrParam());

	for(unsigned long i = 0; i < rdPtr->List().size(); ++i)
	{
		if(tr1::regex_search(rdPtr->List().at(i).c_str(), RegEx))
		{
			return(true);
		}
	}
	return(false);
}

// ============================================================================
//
// CONDITIONS === Linked Lists
// 
// ============================================================================

CONDITION(
	/* ID */			4,
	/* Name */			"%o: Iterator is at beginning",
	/* Flags */			EVFLAGS_ALWAYS|EVFLAGS_NOTABLE,
	/* Params */		(0)
) {
	return(rdPtr->LLit() == rdPtr->LList().begin());
}
CONDITION(
	/* ID */			5,
	/* Name */			"%o: Iterator is at end",
	/* Flags */			EVFLAGS_ALWAYS|EVFLAGS_NOTABLE,
	/* Params */		(0)
) {
	return(rdPtr->LLit() == rdPtr->LList().end());
}

// ============================================================================
//
// ACTIONS === Contiguous Lists
// 
// ============================================================================

ACTION(
	/* ID */			0,
	/* Name */			"Insert Item %0 before %1, %2 times",
	/* Flags */			0,
	/* Params */		(3,PARAM_STRING,"Item String",PARAM_NUMBER,"Insert Before (-1 for end, faster)",PARAM_NUMBER,"Number of Times to Insert")
) {
	string Item = StrParam();
	unsigned long Before = (unsigned long)lParam(); Before = MIN(Before, rdPtr->List().size());
	unsigned long Times = (unsigned long)lParam();
TRY	rdPtr->List().insert(rdPtr->List().begin()+Before, Times, Item); CATCH
}

ACTION(
	/* ID */			1,
	/* Name */			"Change Item %0 to %1",
	/* Flags */			0,
	/* Params */		(2,PARAM_NUMBER,"Index",PARAM_STRING,"New Item String")
) {
	unsigned long Index = (unsigned long)lParam(); Index = MIN(Index, rdPtr->List().size()-1);
	string Item = StrParam();
	if(rdPtr->List().size() == 0){ return; }
TRY	rdPtr->List().at(Index) = Item; CATCH
}

/* ******* ******* ******* ******* ******* ******* ******* */

ACTION(
	/* ID */			2,
	/* Name */			"Copy Items [%0, %1) to %2, Overwrite = %3",
	/* Flags */			0,
	/* Params */		(4,PARAM_NUMBER,"Start (included)",PARAM_NUMBER,"End (excluded)",PARAM_NUMBER,"Copy to before",PARAM_NUMBER,"0 = Insert, anything else = Overwrite")
) {
	unsigned long Start = (unsigned long)lParam(); Start = MIN(Start, rdPtr->List().size()-1);
	unsigned long End = (unsigned long)lParam(); End = MAX(End, Start+1);
	unsigned long To = (unsigned long)lParam(); To = MIN(To, rdPtr->List().size());
	bool Overwrite = (lParam() == 0 ? false : true);
	if(rdPtr->List().size() == 0){ return; }

	vector<string> Temp;
	Temp.insert(Temp.begin(), rdPtr->List().begin()+Start, rdPtr->List().begin()+End);
	if(Overwrite && To < rdPtr->List().size())
	{
		rdPtr->List().erase(rdPtr->List().begin()+To, rdPtr->List().begin()+MIN(To+(End-Start), rdPtr->List().size()));
	}
TRY	rdPtr->List().insert(rdPtr->List().begin()+To, Temp.begin(), Temp.end()); CATCH
}

/* ******* ******* ******* ******* ******* ******* ******* */

ACTION(
	/* ID */			3,
	/* Name */			"Delete Items [%0, %1)",
	/* Flags */			0,
	/* Params */		(2,PARAM_NUMBER,"Start (included)",PARAM_NUMBER,"End (excluded)")
) {
	unsigned long Start = (unsigned long)lParam(); Start = MIN(Start, rdPtr->List().size()-1);
	unsigned long End = (unsigned long)lParam(); End = MAX(End, Start+1);
	if(rdPtr->List().size() == 0){ return; }
TRY	rdPtr->List().erase(rdPtr->List().begin()+Start, rdPtr->List().begin()+End); CATCH
}

ACTION(
	/* ID */			4,
	/* Name */			"Clear List",
	/* Flags */			0,
	/* Params */		(0)
) {
	if(rdPtr->List().size() == 0){ return; }
TRY	rdPtr->List().resize(0); CATCH
}

/* ******* ******* ******* ******* ******* ******* ******* */

ACTION(
	/* ID */			5,
	/* Name */			"Save List to File %0 with item Delimiter %1 and Escape with %2",
	/* Flags */			0,
	/* Params */		(3,PARAM_STRING,"File Path to Save to",PARAM_STRING,"Item Delimiter (escaped from items)",PARAM_STRING,"Escape Character(s) (escaped from items)")
) {
	string File = StrParam();
	string Delimiter = StrParam();
	string Esc = StrParam();
#ifndef VITALIZE
	if(rdPtr->List().size() == 0){ return; }

	ofstream Out (File.c_str(), ios_base::out|ios_base::binary|ios_base::trunc);
	if(!Out.is_open()){ return; }
	string Output;
	for(unsigned long i = 0; i < rdPtr->List().size(); ++i)
	{
		if(i != 0){ Output += Delimiter; }
		Output += Escape(rdPtr->List().at(i), Delimiter, Esc);
	}
	Out.write(Output.c_str(), Output.length());
#endif //VITALIZE
}

ACTION(
	/* ID */			6,
	/* Name */			"Load List from File %0 with item Delimiter %1 and Escape with %2, and Insert before %3",
	/* Flags */			0,
	/* Params */		(4,PARAM_STRING,"File Path to Load from",PARAM_STRING,"Item Delimiter (escaped from items)",PARAM_STRING,"Escape Character(s) (escaped from items)",PARAM_NUMBER,"Insert Before")
) {
	string File = StrParam();
	string Delimiter = StrParam();
	string Esc = StrParam();
	unsigned long Insert = lParam(); Insert = MIN(Insert, rdPtr->List().size());

#ifndef VITALIZE
	ifstream In (File.c_str(), ios_base::in|ios_base::binary);
	if(!In.is_open()){ return; }
	string Input;
	while(In.good() && In.peek() != EOF)
	{
		Input += char(In.get());
	}
	In.close();

/*	string Item;
	for(unsigned long i = 0; i < Input.length(); ++i)
	{
		if(SubStr(Input, i, Delimiter.length()) == Delimiter)
		{
TRY			rdPtr->List().insert(rdPtr->List().begin()+Insert, Item); CATCH
			++Insert;
			Item = "";
			i += Delimiter.length() - 1;
		}
		else if(SubStr(Input, i, MAX(Esc.length(), 1)) == Esc)
		{
			if(SubStr(Input, i+MAX(Esc.length(), 1), Delimiter.length()) == Delimiter)
			{
				Item += Delimiter;
				i += MAX(Esc.length(), 1) + Delimiter.length() - 1;
			}
			else if(SubStr(Input, i+MAX(Esc.length(), 1), MAX(Esc.length(), 1)) == Esc)
			{
				Item += Esc;
				i += MAX(Esc.length(), 1) + MAX(Esc.length(), 1) - 1;
			}
		}
		else
		{
			Item += Input.at(i);
		}
	}
TRY	rdPtr->List().insert(rdPtr->List().begin()+Insert, Item); CATCH
*/
	vector<string> t = Split(Input, Delimiter, Esc);
	rdPtr->List().insert(rdPtr->List().begin()+Insert, t.begin(), t.end());
#endif //VITALIZE
}

ACTION(
	/* ID */			7,
	/* Name */			"Load List from String %0 with item Delimiter %1 and Escape with %2, and Insert before %3",
	/* Flags */			0,
	/* Params */		(4,PARAM_STRING,"String to Load from",PARAM_STRING,"Item Delimiter (escaped from items)",PARAM_STRING,"Escape Character(s) (escaped from items)",PARAM_NUMBER,"Insert Before")
) {
	string Input = StrParam();
	string Delimiter = StrParam();
	string Esc = StrParam();
	unsigned long Insert = lParam(); Insert = MIN(Insert, rdPtr->List().size());

/*	string Item;
	for(unsigned long i = 0; i < Input.length(); ++i)
	{
		if(SubStr(Input, i, Delimiter.length()) == Delimiter)
		{
TRY			rdPtr->List().insert(rdPtr->List().begin()+Insert, Item); CATCH
			++Insert;
			Item = "";
			i += Delimiter.length() - 1;
		}
		else if(SubStr(Input, i, MAX(Esc.length(), 1)) == Esc)
		{
			if(SubStr(Input, i+MAX(Esc.length(), 1), Delimiter.length()) == Delimiter)
			{
				Item += Delimiter;
				i += MAX(Esc.length(), 1) + Delimiter.length() - 1;
			}
			else if(SubStr(Input, i+MAX(Esc.length(), 1), MAX(Esc.length(), 1)) == Esc)
			{
				Item += Esc;
				i += MAX(Esc.length(), 1) + MAX(Esc.length(), 1) - 1;
			}
		}
		else
		{
			Item += Input.at(i);
		}
	}
TRY	rdPtr->List().insert(rdPtr->List().begin()+Insert, Item); CATCH
*/
	vector<string> t = Split(Input, Delimiter, Esc);
	rdPtr->List().insert(rdPtr->List().begin()+Insert, t.begin(), t.end());
}

/* ******* ******* ******* ******* ******* ******* ******* */

ACTION(
	/* ID */			8,
	/* Name */			"Sort List in range [%0, %1) with Sort Order %2",
	/* Flags */			0,
	/* Params */		(3,PARAM_NUMBER,"Start (included)",PARAM_NUMBER,"End (excluded)",PARAM_NUMBER,"0 = Foward; 1 = Reverse; 2 = Custom; 3 = Forward Alphanum; 4 = Reverse Alphanum")
) {
	unsigned long Start = (unsigned long)lParam(); Start = MIN(Start, rdPtr->List().size()-1);
	unsigned long End = (unsigned long)lParam(); End = MAX(End, Start+1);
	unsigned long Sort = (unsigned long)lParam();
	if(rdPtr->List().size() < 2){ return; }
	if(Sort == 0)
	{
		sort(rdPtr->List().begin()+Start, rdPtr->List().begin()+End, ForwardSort);
	}
	else if(Sort == 1)
	{
		sort(rdPtr->List().begin()+Start, rdPtr->List().begin()+End, ReverseSort);
	}
	else if(Sort == 2)
	{
		sort(rdPtr->List().begin()+Start, rdPtr->List().begin()+End, CSO(rdPtr));
	}
	else if(Sort == 3)
	{
		sort(rdPtr->List().begin()+Start, rdPtr->List().begin()+End, ForwardAlphanumSort);
	}
	else if(Sort == 4)
	{
		sort(rdPtr->List().begin()+Start, rdPtr->List().begin()+End, ReverseAlphanumSort);
	}
}

ACTION(
	/* ID */			9,
	/* Name */			"Set Custom Sort Return to %0",
	/* Flags */			0,
	/* Params */		(1,PARAM_NUMBER,"0 = false, anything else = true")
) {
	if(lParam())
	{
		rdPtr->CSr = true;
	}
	else
	{
		rdPtr->CSr = false;
	}
}

/* ******* ******* ******* ******* ******* ******* ******* */

ACTION(
	/* ID */			10,
	/* Name */			"Reserve space for at least %0 total items",
	/* Flags */			0,
	/* Params */		(1,PARAM_NUMBER,"Minimum Space Wanted")
) {
TRY	rdPtr->List().reserve((unsigned long)lParam()); CATCH
}

/* ******* ******* ******* Regular Expressions ******* ******* ******* */

ACTION(
	/* ID */			11,
	/* Name */			"Delete items matching %0",
	/* Flags */			0,
	/* Params */		(1,PARAM_STRING,"Regular Expression String")
) {
	tr1::regex RegEx (StrParam());

	for(unsigned long i = 0; i < rdPtr->List().size(); ++i)
	{
		if(tr1::regex_search(rdPtr->List().at(i).c_str(), RegEx))
		{
TRY			rdPtr->List().erase(rdPtr->List().begin()+(i--)); CATCH
		}
	}
}

/* ******* ******* ******* Multilist ******* ******* ******* */

ACTION(
	/* ID */			12,
	/* Name */			"Change editing list to %0",
	/* Flags */			0,
	/* Params */		(1,PARAM_STRING,"Contiguous List Name")
) {
	rdPtr->CurrentList = StrParam();
}

ACTION(
	/* ID */			13,
	/* Name */			"Delete list %0",
	/* Flags */			0,
	/* Params */		(1,PARAM_STRING,"Contiguous List Name")
) {
	string ListName = StrParam();
	if(ListName == rdPtr->CurrentList){ rdPtr->CurrentList = ""; }
	if(rdPtr->Lists.find(ListName) != rdPtr->Lists.end())
	{
		rdPtr->Lists.erase(ListName);
	}
}

ACTION(
	/* ID */			14,
	/* Name */			"Copy Items from Contiguous List %0 range [%1, %2) to %3, Overwrite = %4",
	/* Flags */			0,
	/* Params */		(5,PARAM_STRING,"Contiguous List to Copy from",PARAM_NUMBER,"Start (included)",PARAM_NUMBER,"End (excluded)",PARAM_NUMBER,"Copy to before",PARAM_NUMBER,"0 = Insert, anything else = Overwrite")
) {
	string FromList = StrParam();
	unsigned long Start = (unsigned long)lParam(); Start = MIN(Start, rdPtr->Lists[FromList].size()-1);
	unsigned long End = (unsigned long)lParam(); End = MAX(End, Start+1);
	unsigned long To = (unsigned long)lParam(); To = MIN(To, rdPtr->List().size());
	bool Overwrite = (lParam() == 0 ? false : true);
	if(rdPtr->Lists[FromList].size() == 0){ return; }

	vector<string> Temp;
	Temp.insert(Temp.begin(), rdPtr->Lists[FromList].begin()+Start, rdPtr->Lists[FromList].begin()+End);
	if(Overwrite && To < rdPtr->List().size())
	{
		rdPtr->List().erase(rdPtr->List().begin()+To, rdPtr->List().begin()+MIN(To+(End-Start), rdPtr->List().size()));
	}
TRY	rdPtr->List().insert(rdPtr->List().begin()+To, Temp.begin(), Temp.end()); CATCH
}

ACTION(
	/* ID */			15,
	/* Name */			"Copy Items from current Linked List in range [%0, %1) to %2",
	/* Flags */			0,
	/* Params */		(3,PARAM_STRING,"Start Iterator Name (included)",PARAM_STRING,"End Iterator Name (excluded, MUST be at least Start+1)",PARAM_NUMBER,"Copy to before")
) {
	string Start = StrParam();
	string End = StrParam();
	unsigned long To = (unsigned long)lParam(); To = MIN(To, rdPtr->List().size());
	if(rdPtr->LList().size() == 0){ return; }

TRY	rdPtr->List().insert(rdPtr->List().begin()+To, rdPtr->LLit(Start), rdPtr->LLit(End)); CATCH
}

// ============================================================================
//
// ACTIONS === Linked Lists
// 
// ============================================================================

ACTION(
	/* ID */			16,
	/* Name */			"[LL] Iterate Forward",
	/* Flags */			0,
	/* Params */		(0)
) {
	if(rdPtr->LLit() != rdPtr->LList().end())
	{
		++rdPtr->LLit();
	}
}

ACTION(
	/* ID */			17,
	/* Name */			"[LL] Iterate Backward",
	/* Flags */			0,
	/* Params */		(0)
) {
	if(rdPtr->LLit() != rdPtr->LList().begin())
	{
		--rdPtr->LLit();
	}
}

ACTION(
	/* ID */			18,
	/* Name */			"[LL] Iterate to Beginning",
	/* Flags */			0,
	/* Params */		(0)
) {
	rdPtr->LLit() = rdPtr->LList().begin();
}

ACTION(
	/* ID */			19,
	/* Name */			"[LL] Iterate to End",
	/* Flags */			0,
	/* Params */		(0)
) {
	rdPtr->LLit() = rdPtr->LList().end();
}

ACTION(
	/* ID */			20,
	/* Name */			"[LL] Insert Item %0, %1 times",
	/* Flags */			0,
	/* Params */		(2,PARAM_STRING,"Item to Insert",PARAM_NUMBER,"Number of Times to Insert")
) {
	string Item = StrParam();
	unsigned long Times = unsigned long(lParam());
TRY	rdPtr->LList().insert(rdPtr->LLit(), Times, Item); CATCH
}

ACTION(
	/* ID */			21,
	/* Name */			"[LL] Change Item to %0",
	/* Flags */			0,
	/* Params */		(1,PARAM_STRING,"New Item String")
) {
	string Item = StrParam();
	if(rdPtr->LList().size() == 0){ return; }
	*rdPtr->LLit() = Item;
}

ACTION(
	/* ID */			22,
	/* Name */			"[LL] Copy Items from range [%0, %1)",
	/* Flags */			0,
	/* Params */		(2,PARAM_STRING,"Start Iterator Name (included)",PARAM_STRING,"End Iterator Name (excluded, MUST be at least Start+1)")
) {
	string Start = StrParam();
	string End = StrParam();
	if(rdPtr->LList().size() == 0){ return; }
TRY	rdPtr->LList().insert(rdPtr->LLit(), rdPtr->LLit(Start), rdPtr->LLit(End)); CATCH
}

ACTION(
	/* ID */			23,
	/* Name */			"[LL] Copy Items from Linked List %0, range [%1, %2)",
	/* Flags */			0,
	/* Params */		(3,PARAM_STRING,"List to Copy from",PARAM_STRING,"Start Iterator Name (included)",PARAM_STRING,"End Iterator Name (excluded, MUST be at least Start+1)")
) {
	string List = StrParam();
	string Start = StrParam();
	string End = StrParam();
	if(rdPtr->LLists[List].size() == 0){ return; }
TRY	rdPtr->LList().insert(rdPtr->LLit(), rdPtr->LLit(Start), rdPtr->LLit(End)); CATCH
}

ACTION(
	/* ID */			24,
	/* Name */			"[LL] Copy Items from current Contiguous List, range [%0, %1)",
	/* Flags */			0,
	/* Params */		(2,PARAM_NUMBER,"Start (included)",PARAM_NUMBER,"End (excluded")
) {
	unsigned long Start = (unsigned long)lParam(); Start = MIN(Start, rdPtr->List().size()-1);
	unsigned long End = (unsigned long)lParam(); End = MAX(End, Start+1);
	if(rdPtr->List().size() == 0){ return; }
TRY	rdPtr->LList().insert(rdPtr->LLit(), rdPtr->List().begin()+Start, rdPtr->List().begin()+End); CATCH
}

ACTION(
	/* ID */			25,
	/* Name */			"[LL] Delete Items in range [%0, %1)",
	/* Flags */			0,
	/* Params */		(2,PARAM_STRING,"Start Iterator Name (included)",PARAM_STRING,"End Iterator Name (excluded, MUST be at least Start+1)")
) {
	string Start = StrParam();
	string End = StrParam();
	if(rdPtr->LList().size() == 0){ return; }
TRY	rdPtr->LList().erase(rdPtr->LLit(Start), rdPtr->LLit(End)); CATCH
	rdPtr->LLit(End) = rdPtr->LLit(Start);
}

ACTION(
	/* ID */			26,
	/* Name */			"[LL] Clear List",
	/* Flags */			0,
	/* Params */		(0)
) {
	rdPtr->LList().clear();
}

/* ******* ******* ******* ******* ******* ******* ******* */

ACTION(
	/* ID */			27,
	/* Name */			"[LL] Save List to File %0 with item Delimiter %1 and Escape with %2",
	/* Flags */			0,
	/* Params */		(3,PARAM_STRING,"File Path to Save to",PARAM_STRING,"Item Delimiter (escaped from items)",PARAM_STRING,"Escape Character(s) (escaped from items)")
) {
	string File = StrParam();
	string Delimiter = StrParam();
	string Esc = StrParam();
#ifndef VITALIZE
	if(rdPtr->LList().size() == 0){ return; }

	ofstream Out (File.c_str(), ios_base::out|ios_base::binary|ios_base::trunc);
	if(!Out.is_open()){ return; }
	string Output;
	for(list<string>::iterator it = rdPtr->LList().begin(); it != rdPtr->LList().end(); ++it)
	{
		if(it != rdPtr->LList().begin()){ Output += Delimiter; }
		Output += Escape(*rdPtr->LLit(), Delimiter, Esc);
	}
	Out.write(Output.c_str(), Output.length());
#endif //VITALIZE
}

ACTION(
	/* ID */			28,
	/* Name */			"[LL] Load List from File %0 with item Delimiter %1 and Escape with %2",
	/* Flags */			0,
	/* Params */		(3,PARAM_STRING,"File Path to Load from",PARAM_STRING,"Item Delimiter (escaped from items)",PARAM_STRING,"Escape Character(s) (escaped from items)")
) {
	string File = StrParam();
	string Delimiter = StrParam();
	string Esc = StrParam();

#ifndef VITALIZE
	ifstream In (File.c_str(), ios_base::in|ios_base::binary);
	if(!In.is_open()){ return; }
	string Input;
	while(In.good() && In.peek() != EOF)
	{
		Input += char(In.get());
	}
	In.close();

	string Item;
	for(unsigned long i = 0; i < Input.length(); ++i)
	{
		if(SubStr(Input, i, Delimiter.length()) == Delimiter)
		{
TRY			rdPtr->LList().insert(rdPtr->LLit(), Item); CATCH
			Item = "";
			i += Delimiter.length() - 1;
		}
		else if(SubStr(Input, i, MAX(Esc.length(), 1)) == Esc)
		{
			if(SubStr(Input, i+MAX(Esc.length(), 1), Delimiter.length()) == Delimiter)
			{
				Item += Delimiter;
				i += MAX(Esc.length(), 1) + Delimiter.length() - 1;
			}
			else if(SubStr(Input, i+MAX(Esc.length(), 1), MAX(Esc.length(), 1)) == Esc)
			{
				Item += Esc;
				i += MAX(Esc.length(), 1) + MAX(Esc.length(), 1) - 1;
			}
		}
		else
		{
			Item += Input.at(i);
		}
	}
TRY	rdPtr->LList().insert(rdPtr->LLit(), Item); CATCH
#endif //VITALIZE
}

ACTION(
	/* ID */			29,
	/* Name */			"[LL] Load List from String %0 with item Delimiter %1 and Escape with %2",
	/* Flags */			0,
	/* Params */		(3,PARAM_STRING,"String to Load from",PARAM_STRING,"Item Delimiter (escaped from items)",PARAM_STRING,"Escape Character(s) (escaped from items)")
) {
	string Input = StrParam();
	string Delimiter = StrParam();
	string Esc = StrParam();

	string Item;
	for(unsigned long i = 0; i < Input.length(); ++i)
	{
		if(SubStr(Input, i, Delimiter.length()) == Delimiter)
		{
TRY			rdPtr->LList().insert(rdPtr->LLit(), Item); CATCH
			Item = "";
			i += Delimiter.length() - 1;
		}
		else if(SubStr(Input, i, MAX(Esc.length(), 1)) == Esc)
		{
			if(SubStr(Input, i+MAX(Esc.length(), 1), Delimiter.length()) == Delimiter)
			{
				Item += Delimiter;
				i += MAX(Esc.length(), 1) + Delimiter.length() - 1;
			}
			else if(SubStr(Input, i+MAX(Esc.length(), 1), MAX(Esc.length(), 1)) == Esc)
			{
				Item += Esc;
				i += MAX(Esc.length(), 1) + MAX(Esc.length(), 1) - 1;
			}
		}
		else
		{
			Item += Input.at(i);
		}
	}
TRY	rdPtr->LList().insert(rdPtr->LLit(), Item); CATCH
}

ACTION(
	/* ID */			30,
	/* Name */			"[LL] Sort List in range [%0, %1) with Sort Order %2",
	/* Flags */			0,
	/* Params */		(3,PARAM_STRING,"Start Iterator Name (included)",PARAM_STRING,"End Iterator Name (excluded, MUST be at least Start+1)",PARAM_NUMBER,"0 = Foward; 1 = Reverse; 2 = Custom; 3 = Forward Alphanum; 4 = Reverse Alphanum")
) {
	string Start = StrParam();
	string End = StrParam();
	unsigned long Sort = (unsigned long)lParam();
	if(rdPtr->List().size() < 2){ return; }
	list<string> temp;
	temp.insert(temp.begin(), rdPtr->LLit(Start), rdPtr->LLit(End));
	if(Sort == 0)
	{
		temp.sort(ForwardSort);
	}
	else if(Sort == 1)
	{
		temp.sort(ReverseSort);
	}
	else if(Sort == 2)
	{
		temp.sort(CSO(rdPtr));
	}
	else if(Sort == 3)
	{
		temp.sort(ForwardAlphanumSort);
	}
	else if(Sort == 4)
	{
		temp.sort(ReverseAlphanumSort);
	}
	for(list<string>::iterator it = rdPtr->LLit(Start), itt = temp.begin(); itt != temp.end(); ++it, ++itt)
	{
		*it = *itt;
	}
}

/* ******* ******* ******* Multilist ******* ******* ******* */

ACTION(
	/* ID */			31,
	/* Name */			"[LL] Change editing list to %0",
	/* Flags */			0,
	/* Params */		(1,PARAM_STRING,"Linked List Name")
) {
	rdPtr->CurrentLList = StrParam();
}

ACTION(
	/* ID */			32,
	/* Name */			"[LL] Change editing iterator to %0",
	/* Flags */			0,
	/* Params */		(1,PARAM_STRING,"Iterator Name")
) {
	rdPtr->CurrentLLit = StrParam();
}

ACTION(
	/* ID */			33,
	/* Name */			"[LL] Delete list %0",
	/* Flags */			0,
	/* Params */		(1,PARAM_STRING,"Linked List Name")
) {
	string ListName = StrParam();
	if(ListName == rdPtr->CurrentLList){ rdPtr->CurrentLList = ""; }
	if(rdPtr->LLists.find(ListName) != rdPtr->LLists.end())
	{
		rdPtr->LLists.erase(ListName);
	}
	if(rdPtr->LLits.find(ListName) != rdPtr->LLits.end())
	{
		rdPtr->LLits.erase(ListName);
	}
}

ACTION(
	/* ID */			34,
	/* Name */			"[LL] Delete iterator %0",
	/* Flags */			0,
	/* Params */		(1,PARAM_STRING,"Iterator Name")
) {
	string ItName = StrParam();
	if(ItName == rdPtr->CurrentLLit){ rdPtr->CurrentLLit = ""; }
	if(rdPtr->LLits[rdPtr->CurrentLList].find(ItName) != rdPtr->LLits[rdPtr->CurrentLList].end())
	{
		rdPtr->LLits[rdPtr->CurrentLList].erase(ItName);
	}
}

// ============================================================================
//
// EXPRESSIONS === Contiguous Lists
// 
// ============================================================================

EXPRESSION(
	/* ID */			0,
	/* Name */			"Item$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(1,EXPPARAM_NUMBER,"Item Index")
) {
	unsigned long Index = (unsigned long)xlParam();
	if(rdPtr->List().size() == 0){ ReturnString(""); }
	string Item = rdPtr->List().at(MIN(Index, rdPtr->List().size()-1));
	ReturnStringSafe(Item.c_str());
}

EXPRESSION(
	/* ID */			1,
	/* Name */			"Items(",
	/* Flags */			0,
	/* Params */		(0)
) {
	return(rdPtr->List().size());
}

/* ******* ******* ******* ******* ******* ******* ******* */

EXPRESSION(
	/* ID */			2,
	/* Name */			"ItemOccurrences(",
	/* Flags */			0,
	/* Params */		(1,EXPPARAM_STRING,"Item String")
) {
	string Item = EStrParam();

	unsigned long c = 0;
	for(unsigned long i = 0; i < rdPtr->List().size(); ++i)
	{
		if(Item == rdPtr->List().at(i))
		{
			++c;
		}
	}
	return(c);
}

EXPRESSION(
	/* ID */			3,
	/* Name */			"IndexByOccurrence(",
	/* Flags */			0,
	/* Params */		(2,EXPPARAM_STRING,"Item String",EXPPARAM_NUMBER,"Occurrence # (1-based)")
) {
	string Item = EStrParam();
	unsigned long Occ = xlParam();

	unsigned long c = 0;
	for(unsigned long i = 0; i < rdPtr->List().size(); ++i)
	{
		if(Item == rdPtr->List().at(i))
		{
			++c;
			if(c == Occ)
			{
				return(i);
			}
		}
	}
	return(-1);
}

/* ******* ******* ******* ******* ******* ******* ******* */

EXPRESSION(
	/* ID */			4,
	/* Name */			"List$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(2,EXPPARAM_STRING,"Item Delimiter (escaped from items)",EXPPARAM_STRING,"Escape Character(s) (escaped from items)")
) {
	string Delimiter = EStrParam();
	string Esc = EStrParam();

	string Final;
	for(unsigned long i = 0; i < rdPtr->List().size(); ++i)
	{
		if(i != 0){ Final += Delimiter; }
		Final += Escape(rdPtr->List().at(i), Delimiter, Esc);
	}
	ReturnStringSafe(Final.c_str());
}

/* ******* ******* ******* ******* ******* ******* ******* */

EXPRESSION(
	/* ID */			5,
	/* Name */			"SortStrA$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(0)
) {
	ReturnStringSafe(rdPtr->CSa.c_str());
}

EXPRESSION(
	/* ID */			6,
	/* Name */			"SortStrB$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(0)
) {
	ReturnStringSafe(rdPtr->CSb.c_str());
}

/* ******* ******* ******* ******* ******* ******* ******* */

EXPRESSION(
	/* ID */			7,
	/* Name */			"Capacity(",
	/* Flags */			0,
	/* Params */		(0)
) {
	return(rdPtr->List().capacity());
}

/* ******* ******* ******* ******* ******* ******* ******* */

EXPRESSION(
	/* ID */			8,
	/* Name */			"Reserved/Unused",
	/* Flags */			0,
	/* Params */		(0)
) {
	return(0);
}

/* ******* ******* ******* Regular Expressions ******* ******* ******* */

EXPRESSION(
	/* ID */			9,
	/* Name */			"RegexItemOccurrences(",
	/* Flags */			0,
	/* Params */		(1,EXPPARAM_STRING,"Regular Expression String")
) {
	tr1::regex RegEx (EStrParam());

	unsigned long c = 0;
	for(unsigned long i = 0; i < rdPtr->List().size(); ++i)
	{
		if(tr1::regex_search(rdPtr->List().at(i).c_str(), RegEx))
		{
			++c;
		}
	}
	return(c);
}

EXPRESSION(
	/* ID */			10,
	/* Name */			"RegexIndexByOccurrence(",
	/* Flags */			0,
	/* Params */		(2,EXPPARAM_STRING,"Regular Expression String",EXPPARAM_NUMBER,"Occurrence # (1-based)")
) {
	tr1::regex RegEx (EStrParam());
	unsigned long Occ = xlParam();

	unsigned long c = 0;
	for(unsigned long i = 0; i < rdPtr->List().size(); ++i)
	{
		if(tr1::regex_search(rdPtr->List().at(i).c_str(), RegEx))
		{
			++c;
			if(c == Occ)
			{
				return(i);
			}
		}
	}
	return(-1);
}

/* ******* ******* ******* Multilist ******* ******* ******* */

EXPRESSION(
	/* ID */			11,
	/* Name */			"CurrentList$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(0)
) {
	ReturnStringSafe(rdPtr->CurrentList.c_str());
}

EXPRESSION(
	/* ID */			12,
	/* Name */			"Lists(",
	/* Flags */			0,
	/* Params */		(0)
) {
	return(rdPtr->Lists.size());
}

EXPRESSION(
	/* ID */			13,
	/* Name */			"ListByIndex$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(1,EXPPARAM_NUMBER,"Index of List")
) {
	unsigned long List = MIN(rdPtr->Lists.size() - 1, unsigned long(xlParam()));
	map<string, vector<string> >::iterator it = rdPtr->Lists.begin();
	if(rdPtr->Lists.size() == 0){ ReturnStringSafe(""); }
	for(; List > 0; --List, ++it){}
	ReturnStringSafe(it->first.c_str());
}

EXPRESSION(
	/* ID */			14,
	/* Name */			"ChangeList$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(1,EXPPARAM_STRING,"List Name")
) {
	rdPtr->CurrentList = EStrParam();
	ReturnStringSafe("");
}

EXPRESSION(
	/* ID */			15,
	/* Name */			"ChangeList(",
	/* Flags */			0,
	/* Params */		(1,EXPPARAM_STRING,"List Name")
) {
	rdPtr->CurrentList = EStrParam();
	return(0);
}

// ============================================================================
//
// EXPRESSIONS === Linked Lists
// 
// ============================================================================

EXPRESSION(
	/* ID */			16,
	/* Name */			"LL Item$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(0)
) {
	if(rdPtr->LList().size() == 0){ ReturnString(""); }
	ReturnStringSafe(rdPtr->LLit()->c_str());
}

EXPRESSION(
	/* ID */			17,
	/* Name */			"LL Items(",
	/* Flags */			0,
	/* Params */		(0)
) {
	return(rdPtr->LList().size());
}

EXPRESSION(
	/* ID */			18,
	/* Name */			"LList$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(2,EXPPARAM_STRING,"Item Delimiter (escaped from items)",EXPPARAM_STRING,"Escape Character(s) (escaped from items)")
) {
	string Delimiter = EStrParam();
	string Esc = EStrParam();

	string Final;
	bool first = true;
	for(list<string>::iterator it = rdPtr->LList().begin(); it != rdPtr->LList().end(); ++it)
	{
		if(!first){ Final += Delimiter; } first = false;
		Final += Escape(*it, Delimiter, Esc);
	}
	ReturnStringSafe(Final.c_str());
}

EXPRESSION(
	/* ID */			19,
	/* Name */			"CurrentLList$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(0)
) {
	ReturnStringSafe(rdPtr->CurrentLList.c_str());
}

EXPRESSION(
	/* ID */			20,
	/* Name */			"CurrentLLIterator$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(0)
) {
	ReturnStringSafe(rdPtr->CurrentLLit.c_str());
}

EXPRESSION(
	/* ID */			21,
	/* Name */			"LLists(",
	/* Flags */			0,
	/* Params */		(0)
) {
	return(rdPtr->LLists.size());
}

EXPRESSION(
	/* ID */			22,
	/* Name */			"LListByIndex$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(1,EXPPARAM_NUMBER,"Index of List")
) {
	unsigned long List = MIN(rdPtr->LLists.size() - 1, unsigned long(xlParam()));
	map<string, list<string> >::iterator it = rdPtr->LLists.begin();
	for(; List > 0; --List, ++it){}
	ReturnStringSafe(it->first.c_str());
}

EXPRESSION(
	/* ID */			23,
	/* Name */			"ChangeLList$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(1,EXPPARAM_STRING,"List Name")
) {
	rdPtr->CurrentLList = EStrParam();
	ReturnStringSafe("");
}

EXPRESSION(
	/* ID */			24,
	/* Name */			"ChangeLList(",
	/* Flags */			0,
	/* Params */		(1,EXPPARAM_STRING,"List Name")
) {
	rdPtr->CurrentLList = EStrParam();
	return(0);
}
