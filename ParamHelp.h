//Float Param Help Functions
inline float gfParam(LPRDATA rdPtr, int param1) {int f = CNC_GetFloatParameter(rdPtr); return *(float*)&f;}
inline float xgfParam(LPRDATA rdPtr, int param1) {int f = ExParam(TYPE_FLOAT); return *(float*)&f;}

/* ******* Actions/Conditions ******* */
//Object Parameter
#define oParam() (LPRO)Param(-1)
//Undefined Parameter
#define uParam() Param(-1)
//Long/Int Parameter
#define lParam() Param(TYPE_INT)
//Float Parameter
#define fParam() gfParam(rdPtr,param1)
//String Parameter
#define sParam() (LPCSTR)Param(TYPE_STRING)

/* ******* Expressions ******* */
//Long/Int Parameter
#define xlParam() ExParam(TYPE_INT)
//String Parameter
#define xsParam() (LPCSTR)ExParam(TYPE_STRING)
//Float Parameter
#define xfParam() xgfParam(rdPtr,param1)

/* ******* LB's std::string macros ******* */
#ifdef _STRING_
	#define StrParam() string((char *)Param(TYPE_STRING))
	#define EStrParam() string((char *)ExParam(TYPE_STRING))
#endif //_STRING_