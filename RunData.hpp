#pragma once

#include "FusionAPI.hpp"
#include "Data.hpp"

struct RunData final
{
	headerObject rHo;
	rVal rv;

	Data *local_data; //owning
	Data::lists_t::key_type *current_list_name; //owning
	Data::global_t::key_type *global_data_name; //owning, optional

	auto data() noexcept
	-> Data &
	{
		if(global_data_name)
		{
			return Data::global[*global_data_name];
		}
		return *local_data;
	}
	auto current()
	-> Data::list_t &
	{
		return data().lists[*current_list_name];
	}
	auto current_optional() noexcept
	-> Data::list_t * //non-owning, optional
	{
		if(global_data_name)
		{
			if(auto const it = Data::global.find(*global_data_name); it != end(Data::global))
			{
				if(auto const jt = it->second.lists.find(*current_list_name); jt != end(it->second.lists))
				{
					return std::addressof(jt->second);
				}
			}
			return nullptr;
		}
		if(auto const it = local_data->lists.find(*current_list_name); it != end(local_data->lists))
		{
			return std::addressof(it->second);
		}
		return nullptr;
	}

	RunData() = delete;
	RunData(RunData const &) = delete;
	RunData(RunData &&) = delete;
	RunData &operator=(RunData &&) = delete;
	RunData &operator=(RunData const &) = delete;
	~RunData() = delete;
};

static_assert(fusion::is_valid_rundata_structure<FUSION_OEFLAGS>());
