#ifndef _StringConverter_HeaderPlusPlus_
#define _StringConverter_HeaderPlusPlus_
#ifdef _STRING_
#include <sstream>
using namespace std;

template<typename T>
class ConvertTo
{
	ConvertTo(){}
	ConvertTo(const ConvertTo<T>&){}
	ConvertTo<T>& operator=(const ConvertTo<T>&){ return(*this); }
public:
	~ConvertTo(){}
	static string Str(const T& from)
	{
		ostringstream ss;
		ss << from;
		return(ss.str());
	}
	static T Num(const string& from)
	{
		istringstream ss (from);
		T temp;
		ss >> temp;
		return(temp);
	}
};
template<>
class ConvertTo<signed char>
{
	ConvertTo(){}
	ConvertTo(const ConvertTo<signed char>&){}
	ConvertTo<signed char>& operator=(const ConvertTo<signed char>&){ return(*this); }
public:
	~ConvertTo(){}
	static string Str(const signed char& from)
	{
		ostringstream ss;
		ss << signed short(from);
		return(ss.str());
	}
	static signed char Num(const string& from)
	{
		istringstream ss (from);
		signed short temp;
		ss >> temp;
		return(signed char(temp));
	}
};
template<>
class ConvertTo<unsigned char>
{
	ConvertTo(){}
	ConvertTo(const ConvertTo<unsigned char>&){}
	ConvertTo<unsigned char>& operator=(const ConvertTo<unsigned char>&){ return(*this); }
public:
	~ConvertTo(){}
	static string Str(const unsigned char& from)
	{
		ostringstream ss;
		ss << unsigned short(from);
		return(ss.str());
	}
	static unsigned char Num(const string& from)
	{
		istringstream ss (from);
		unsigned short temp;
		ss >> temp;
		return(unsigned char(temp));
	}
};

#endif //_STRING_
#endif //_StringConverter_HeaderPlusPlus_