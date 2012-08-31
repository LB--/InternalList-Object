/// =====================================================================================
//
// The following routines are used internally by MMF, and should not need to be modified
// 
// =====================================================================================

#include "common.h"

HINSTANCE hInstLib;
EXT_INIT()

// ============================================================================
//
// LIBRARY ENTRY & QUIT POINTS
// 
// ============================================================================

// -----------------
// Entry points
// -----------------
// Usually you do not need to do any initialization here: it is preferable to
// do it in "Initialize" found in Edittime.cpp

BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
{

	conditionsInfos = getConditionInfos();
	actionsInfos = getActionInfos();
	expressionsInfos = getExpressionInfos();
	
	ConditionJumps = getConditions();
	ActionJumps = getActions();
	ExpressionJumps = getExpressions();
	
	switch (dwReason)
	{
		// DLL is attaching to the address space of the current process.
		case DLL_PROCESS_ATTACH:
			
			hInstLib = hDLL; // Store HINSTANCE
			break;

		// A new thread is being created in the current process.
		case DLL_THREAD_ATTACH:
			break;

		// A thread is exiting cleanly.
		case DLL_THREAD_DETACH:
			break;

		// The calling process is detaching the DLL from its address space.
	    case DLL_PROCESS_DETACH:
			break;
	}
	
	return TRUE;
}

// -----------------
// Initialize
// -----------------
// Where you want to do COLD-START initialization.
// Called when the extension is loaded into memory.
//
extern "C" int WINAPI DLLExport Initialize(mv _far *mV, int quiet)
{
	// No error
	return 0;
}

// -----------------
// Free
// -----------------
// Where you want to kill and initialized data opened in the above routine
// Called just before freeing the DLL.
// 
extern "C" int WINAPI DLLExport Free(mv _far *mV)
{
	// No error
	return 0;
}

// ============================================================================
//
// GENERAL INFO
// 
// ============================================================================

// -----------------
// Get Infos
// -----------------
// 
extern "C" 
{
	DWORD WINAPI DLLExport GetInfos(int info)
	{
		
		switch (info)
		{
			case KGI_VERSION:
				return EXT_VERSION2;
			case KGI_PLUGIN:
				return EXT_PLUGIN_VERSION1;
			case KGI_PRODUCT:
				return ForVersion;
			case KGI_BUILD:
				return MinimumBuild;
			default:
				return 0;
		}
	}
}

// ----------------------------------------------------------
// GetRunObjectInfos
// ----------------------------------------------------------
// Fills an information structure that tells MMF2 everything
// about the object, its actions, conditions and expressions
// 

short WINAPI DLLExport GetRunObjectInfos(mv _far *mV, fpKpxRunInfos infoPtr)
{
	infoPtr->conditions = (LPBYTE)ConditionJumps;
	infoPtr->actions = (LPBYTE)ActionJumps;
	infoPtr->expressions = (LPBYTE)ExpressionJumps;

	infoPtr->numOfConditions = Conditions.size();
	infoPtr->numOfActions = Actions.size();
	infoPtr->numOfExpressions = Expressions.size();

	infoPtr->editDataSize = sizeof(EDITDATA);
	
	MagicFlags(infoPtr->editFlags);

	infoPtr->windowProcPriority = WINDOWPROC_PRIORITY;

	MagicPrefs(infoPtr->editPrefs);

	infoPtr->identifier = IDENTIFIER;
	infoPtr->version = 3; ///////////////////////////////////////////////////////////////////////////////////Extension Version!!

	return TRUE;
}

// ----------------------------------------------------------
// GetDependencies
// ----------------------------------------------------------
// Returns the name of the external modules that you wish MMF to include
// with stand-alone applications (these modules must be in the MMF
// Data\Runtime folder).
//

LPCSTR* WINAPI DLLExport GetDependencies()
{
	// Do some rSDK stuff
	#include "rGetDependencies.h"
	
	//LPCSTR szDep[] = {
	//	"MyDll.dll",
	//	NULL
	//};

	return NULL; // szDep;
}

// -----------------
// LoadObject
// -----------------
// Routine called for each object when the object is read from the MFA file (edit time)
// or from the CCN or EXE file (run time).
// You can load data here, reserve memory etc...
//
int	WINAPI DLLExport LoadObject(mv _far *mV, LPCSTR fileName, LPEDATA edPtr, int reserved)
{
/*	if(edPtr->eHeader.extVersion == 1) //Version before the string properties were added
	{
		edPtr = (LPEDATA)mvReAllocEditData(mV, edPtr, sizeof(EDITDATA)+3);
		(&edPtr->Data)[0] = 0;
		(&edPtr->Data)[1] = 0;
		edPtr->LDOffset = 1;
		(&edPtr->Data)[2] = 0;
		edPtr->IDOffset = 2;
		(&edPtr->Data)[3] = 0;
		edPtr->ESOffset = 3;
		edPtr->eHeader.extVersion = 3; //Done updating
	}
	else if(edPtr->eHeader.extVersion == 2) //Version before Jamie's request...
	{
		//Do nothing...
		edPtr->eHeader.extVersion = 3;
	}
	else if(edPtr->eHeader.extVersion > 3)
	{
		MessageBox(NULL, "This MFA was saved with a newer version of the Internal List Object and may not load correctly.", "Warning - Internal List Object", MB_OK);
	}
	else if(edPtr->eHeader.extVersion < 1)
	{
		MessageBox(NULL, "This MFA may be corrupt! The version number is a nonexistant version (not a newer version).", "Warning - Internal List Object", MB_OK);
	}
*/
	EditData(edPtr, mV).Serialize(edPtr, mV);
	return 0;
}

// -----------------
// UnloadObject
// -----------------
// The counterpart of the above routine: called just before the object is
// deleted from the frame.
//
void WINAPI DLLExport UnloadObject(mv _far *mV, LPEDATA edPtr, int reserved)
{
}

// --------------------
// UpdateEditStructure
// --------------------
// For you to update your object structure to newer versions
// Called at both edit time and run time
// 
HGLOBAL WINAPI DLLExport UpdateEditStructure(mv __far *mV, void __far * OldEdPtr)
{
	// We do nothing here
	return 0;
}

// --------------------
// UpdateFileNames
// --------------------
// If you store file names in your datazone, they have to be relocated when the
// application is moved to a different directory: this routine does it.
// Called at edit time and run time.
//
// Call lpfnUpdate to update your file pathname (refer to the documentation)
// 
void WINAPI DLLExport UpdateFileNames(mv _far *mV, LPSTR appName, LPEDATA edPtr, void (WINAPI * lpfnUpdate)(LPSTR, LPSTR))
{
}

// ---------------------
// EnumElts
// ---------------------
//
// Uncomment this function if you need to store an image in the image bank.
//
// Note: do not forget to enable the function in the .def file 
// if you remove the comments below.
//
/*
int WINAPI DLLExport EnumElts (mv __far *mV, LPEDATA edPtr, ENUMELTPROC enumProc, ENUMELTPROC undoProc, LPARAM lp1, LPARAM lp2)
{  
	int error = 0;

	// Replace wImgIdx with the name of the WORD variable you create within the edit structure
  
	// Enum images  
	if ( (error = enumProc(&edPtr->wImgIdx, IMG_TAB, lp1, lp2)) != 0 )
	{
		// Undo enum images      
		undoProc (&edPtr->wImgIdx, IMG_TAB, lp1, lp2);    
	}  

	return error;
}
*/

void WINAPI PrepareFlexBuild(LPMV pMV, LPEDATA edPtr, LPCWSTR wTempFolder)
{
#ifndef RUN_ONLY

	//

#endif // !defined(RUN_ONLY)
}