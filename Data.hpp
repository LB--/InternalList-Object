#pragma once

#include <map>
#include <string>
#include <vector>

#include "lSDK/UnicodeUtilities.hpp"

struct Data final
{
	using list_t = std::vector<lSDK::string_t>;
	using lists_t = std::map<lSDK::string_t, list_t, std::less<>>;
	lists_t lists;

	using global_t = std::map<lSDK::string_t, Data, std::less<>>;
	static global_t global;
};

inline auto escape_item(lSDK::string_view_t item, lSDK::string_view_t const to, lSDK::string_view_t const esc)
{
	lSDK::string_t temp;
	while(!empty(item))
	{
		if(!empty(to) && item.substr(0, size(to)) == to)
		{
			temp.append(esc);
			temp.append(to);
			item.remove_prefix(size(to));
		}
		else if(!empty(esc) && item.substr(0, size(esc)) == esc)
		{
			temp.append(esc);
			temp.append(esc);
			item.remove_prefix(size(esc));
		}
		else
		{
			temp.append(item.substr(0, 1));
			item.remove_prefix(1);
		}
	}
	return temp;
}
inline auto combine_escaped(Data::list_t const &list, lSDK::string_view_t const delim, lSDK::string_view_t const esc)
-> lSDK::string_t
{
	lSDK::string_t text;
	for(std::size_t i = 0; i < size(list); ++i)
	{
		if(i != 0)
		{
			text.append(delim);
		}
		text.append(escape_item(list[i], delim, esc));
	}
	return text;
}
template<typename Callable>
constexpr void split_escaped(lSDK::string_view_t str, lSDK::string_view_t const delim, lSDK::string_view_t const esc, Callable &&item_handler)
{
	lSDK::string_view_t temp = str;
	while(!empty(str))
	{
		if(!empty(esc) && str.substr(0, size(esc)) == esc)
		{
			if(str.substr(size(esc), size(delim)) == delim)
			{
				str.remove_prefix(size(esc) + size(delim));
			}
			else if(str.substr(size(esc), size(esc)) == esc)
			{
				str.remove_prefix(size(esc)*2);
			}
			else
			{
				str.remove_prefix(size(esc));
			}
		}
		else if(!empty(delim) && str.substr(0, size(delim)) == delim)
		{
			std::ptrdiff_t const len = data(str) - data(temp);
			item_handler(temp.substr(0, static_cast<std::size_t>(len)));
			str.remove_prefix(size(delim));
			temp = str;
		}
		else
		{
			str.remove_prefix(1);
		}
	}
	item_handler(temp);
}
