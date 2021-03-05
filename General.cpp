#include "EditData.hpp"
#include "FusionAPI.hpp"
#include "RunData.hpp"

#include <array>

#ifdef FUSION_GET_COMPATIBILITY
auto FUSION_API GetInfos(std::int32_t const which) noexcept
-> std::uint32_t
{
	switch(which)
	{
	case KGI_VERSION:
		return FUSION_GET_COMPATIBILITY_VERSION;
	case KGI_PLUGIN:
		return FUSION_GET_COMPATIBILITY_PLUGIN;
	case KGI_PRODUCT:
		return PRODUCT_VERSION_STANDARD;
	case KGI_BUILD:
		return 292;
	case KGI_UNICODE:
		return FUSION_GET_COMPATIBILITY_UNICODE;
	}
	return 0;
}
#endif

#ifdef FUSION_ACTIONS
fusion::action_func
	action_insert_item_times,
	action_change_item,
	action_copy_items,
	action_delete_items,
	action_clear_list,
	action_save_to_file,
	action_load_from_file,
	action_load_from_string,
	action_sort_range,
	action_custom_sort_return,
	action_reserve_space,
	action_regex_erase,
	action_change_current_list,
	action_delete_list,
	action_copy_items_from_list,
	action_insert_item,
	action_use_global_data,
	action_use_local_data,
	action_copy_local_to_global,
	action_copy_global_to_local,
	action_delete_global_data,
	action_delete_local_data;
#endif
#ifdef FUSION_CONDITIONS
fusion::condition_func
	condition_item_in_list,
	condition_on_trigger,
	condition_regex_in_list;
#endif
#ifdef FUSION_EXPRESSIONS
fusion::expression_func
	expression_item_by_index,
	expression_number_of_items,
	expression_count_item,
	expression_nth_item_index,
	expression_stringify,
	expression_custom_sort_a,
	expression_custom_sort_b,
	expression_capacity,
	expression_count_regex,
	expression_nth_regex_index,
	expression_current_list,
	expression_number_of_lists,
	expression_list_name_by_index,
	expression_change_current_list_s,
	expression_change_current_list_i,
	expression_error_message;
#endif

template<std::size_t NUM_PARAMS>
auto FUSION_API unimplemented_action(RunData *const run_data, fusion::ac_param_t const, fusion::ac_param_t const) noexcept
-> fusion::action_return_t
{
	if constexpr(NUM_PARAMS > 2)
	{
		for(std::size_t i = 0; i < NUM_PARAMS; ++i)
		{
			(void)CNC_GetParameter(run_data);
		}
	}
	return 0;
}

template<std::size_t NUM_PARAMS>
auto FUSION_API unimplemented_condition(RunData *const run_data, fusion::ac_param_t const, fusion::ac_param_t const) noexcept
-> fusion::condition_return_t
{
	if constexpr(NUM_PARAMS > 2)
	{
		for(std::size_t i = 0; i < NUM_PARAMS; ++i)
		{
			(void)CNC_GetParameter(run_data);
		}
	}
	return 0;
}

template<int RETURN_TYPE, int... PARAM_TYPES>
auto FUSION_API unimplemented_expression(RunData *const run_data, fusion::e_params_t const params_handle) noexcept
-> fusion::expression_return_t
{
	if constexpr(sizeof...(PARAM_TYPES) > 0)
	{
		static constexpr int PARAM_TYPES_ARRAY[]{PARAM_TYPES...};
		(void)CNC_GetFirstExpressionParameter(run_data, params_handle, (PARAM_TYPES_ARRAY[0]));
		if constexpr(sizeof...(PARAM_TYPES) > 1)
		{
			for(std::size_t i = 1; i < sizeof...(PARAM_TYPES); ++i)
			{
				(void)CNC_GetNextExpressionParameter(run_data, params_handle, (PARAM_TYPES_ARRAY[i]));
			}
		}
	}

	if constexpr(RETURN_TYPE == TYPE_FLOAT)
	{
		run_data->rHo.hoFlags |= HOF_FLOAT;
		static constexpr float FZERO = 0.f;
		return *reinterpret_cast<std::uint32_t const *>(&FZERO);
	}
	else if constexpr(RETURN_TYPE == TYPE_STRING)
	{
		run_data->rHo.hoFlags |= HOF_STRING;
		return reinterpret_cast<fusion::expression_return_t>(TSL(""));
	}
	return 0;
}

#ifdef FUSION_GET_EXTENSION_INFO
auto FUSION_API GetRunObjectInfos(mv *const mV, kpxRunInfos *const run_info) noexcept
-> std::int16_t
{
	if(!run_info)
	{
		return FUSION_GET_EXTENSION_INFO_FAILURE;
	}

	run_info->identifier = *reinterpret_cast<long const *>("ITLO");
	run_info->version = SerializedEditData::CURRENT_VERSION;

	run_info->editDataSize = sizeof(SerializedEditData);

	run_info->windowProcPriority = 100;

	run_info->editFlags = FUSION_OEFLAGS;
	run_info->editPrefs = 0;

	static fusion::    action_func_pointer     actions[] =
	{
		/*  0 */ action_insert_item_times,
		/*  1 */ action_change_item,
		/*  2 */ action_copy_items,
		/*  3 */ action_delete_items,
		/*  4 */ action_clear_list,
		/*  5 */ action_save_to_file,
		/*  6 */ action_load_from_file,
		/*  7 */ action_load_from_string,
		/*  8 */ action_sort_range,
		/*  9 */ action_custom_sort_return,
		/* 10 */ action_reserve_space,
		/* 11 */ action_regex_erase,
		/* 12 */ action_change_current_list,
		/* 13 */ action_delete_list,
		/* 14 */ action_copy_items_from_list,
		/* 15 */ unimplemented_action<3>,
		/* 16 */ unimplemented_action<0>,
		/* 17 */ unimplemented_action<0>,
		/* 18 */ unimplemented_action<0>,
		/* 19 */ unimplemented_action<0>,
		/* 20 */ unimplemented_action<2>,
		/* 21 */ unimplemented_action<1>,
		/* 22 */ unimplemented_action<2>,
		/* 23 */ unimplemented_action<3>,
		/* 24 */ unimplemented_action<2>,
		/* 25 */ unimplemented_action<2>,
		/* 26 */ unimplemented_action<0>,
		/* 27 */ unimplemented_action<3>,
		/* 28 */ unimplemented_action<3>,
		/* 29 */ unimplemented_action<3>,
		/* 30 */ unimplemented_action<3>,
		/* 31 */ unimplemented_action<1>,
		/* 32 */ unimplemented_action<1>,
		/* 33 */ unimplemented_action<1>,
		/* 34 */ unimplemented_action<1>,
		/* 35 */ action_insert_item,
		/* 36 */ action_use_global_data,
		/* 37 */ action_use_local_data,
		/* 38 */ action_copy_local_to_global,
		/* 39 */ action_copy_global_to_local,
		/* 40 */ action_delete_global_data,
		/* 41 */ action_delete_local_data,
	};
	static fusion:: condition_func_pointer  conditions[] =
	{
		/*  0 */ condition_item_in_list,
		/*  1 */ condition_on_trigger,
		/*  2 */ condition_on_trigger,
		/*  3 */ condition_regex_in_list,
		/*  4 */ unimplemented_condition<0>,
		/*  5 */ unimplemented_condition<0>,
	};
	static fusion::expression_func_pointer expressions[] =
	{
		/*  0 */ expression_item_by_index,
		/*  1 */ expression_number_of_items,
		/*  2 */ expression_count_item,
		/*  3 */ expression_nth_item_index,
		/*  4 */ expression_stringify,
		/*  5 */ expression_custom_sort_a,
		/*  6 */ expression_custom_sort_b,
		/*  7 */ expression_capacity,
		/*  8 */ unimplemented_expression<TYPE_INT>,
		/*  9 */ expression_count_regex,
		/* 10 */ expression_nth_regex_index,
		/* 11 */ expression_current_list,
		/* 12 */ expression_number_of_lists,
		/* 13 */ expression_list_name_by_index,
		/* 14 */ expression_change_current_list_s,
		/* 15 */ expression_change_current_list_i,
		/* 16 */ unimplemented_expression<TYPE_STRING>,
		/* 17 */ unimplemented_expression<TYPE_INT>,
		/* 18 */ unimplemented_expression<TYPE_STRING, TYPE_STRING, TYPE_STRING>,
		/* 19 */ unimplemented_expression<TYPE_STRING>,
		/* 20 */ unimplemented_expression<TYPE_STRING>,
		/* 21 */ unimplemented_expression<TYPE_INT>,
		/* 22 */ unimplemented_expression<TYPE_STRING, TYPE_INT>,
		/* 23 */ unimplemented_expression<TYPE_STRING, TYPE_STRING>,
		/* 24 */ unimplemented_expression<TYPE_INT, TYPE_STRING>,
		/* 25 */ expression_error_message,
	};

	run_info->    actions = static_cast<fusion::    action_func_array>(    actions);
	run_info-> conditions = static_cast<fusion:: condition_func_array>( conditions);
	run_info->expressions = static_cast<fusion::expression_func_array>(expressions);

	run_info->numOfActions     = static_cast<short>(std::size(    actions));
	run_info->numOfConditions  = static_cast<short>(std::size( conditions));
	run_info->numOfExpressions = static_cast<short>(std::size(expressions));

	return FUSION_GET_EXTENSION_INFO_SUCCESS;
}
#endif
