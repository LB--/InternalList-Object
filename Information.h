// --------------------
// Object information
// --------------------

// Basic object details
#define	ObjectName			"Internal List Object"
#define	ObjectAuthor		"LB"
#define	ObjectCopyright		"Copyright © 2012 LB-Stuff"
#define	ObjectComment		"Allows you to utilize an invisible list object with many more features than the List object.\nIt is not a Windows control so it does not slow your application down as much as the List object would.\nAlso supports multiple lists of two types, regular expressions, and advanced sorting.\nIcon thanks to Jaffob."
#define	ObjectURL			"http://www.LB-Stuff.com/"
#define	ObjectHelp			""

// If you register your object with Clickteam, change this to the ID you were given
#define ObjectRegID			REGID_PRIVATE

// Change N,O,N,E to 4 unique characters (MMF currently still uses this to keep track)
#define	IDENTIFIER			MAKEID(I,T,L,O) //InTernal List Object

// --------------------
// Version information
// --------------------

// PRODUCT_VERSION_TGF or PRODUCT_VERSION_DEVELOPER
#define ForVersion			PRODUCT_VERSION_STANDARD

// Set this to the latest MMF build out when you build the object
#define	MinimumBuild		254

// --------------------
// Beta information
// --------------------

 #define BETA
 #define POPUP_ON_DROP
// #define POPUP_ON_EXE
// #define POPUP_ON_BUILD
 #define POPUP_MESSAGE	"This is a beta version; use with caution!\nIf you find a crash, bug, or any other error, please report it with a minimal example/instructions to reproduce it."

// --------------------
// Handling priority
// --------------------
// If this extension will handle windows messages, specify the priority
// of the handling procedure, 0 = low and 255 = very high

#define	WINDOWPROC_PRIORITY 100
