#include "FusionAPI.hpp"
#include "lSDK.hpp"
#include "lSDK/WindowsUtilities.hpp"
#include "RunData.hpp"

#include <algorithm>
#include <memory>
#include <regex>
#include <variant>

using fusion::action_return_t;
using fusion::condition_return_t;
using fusion::expression_return_t;
using fusion::ac_param_t;
using fusion::e_params_t;

using lSDK::char_t;
using lSDK::string_t;
using lSDK::string_view_t;

struct TriggerOnCustomSortCompare final
{
	static constexpr std::int16_t Id = 1;
	char_t const *const a {};
	char_t const *const b {};
	bool result {};
};
struct TriggerOnError final
{
	static constexpr std::int16_t Id = 2;
	void const *ace {};
	std::exception const *e {};
	mutable std::unique_ptr<char_t[]> cached_message;
};

template<typename... T>
using ref_variant = std::variant<std::reference_wrapper<T>...>;
using TriggerParam_t = ref_variant
<
	TriggerOnCustomSortCompare,
	TriggerOnError
>;
template<typename ParamT>
static void trigger_condition(headerObject &header_object, std::int16_t condition_id, ParamT &param)
{
	TriggerParam_t wrapper {std::in_place_type_t<std::reference_wrapper<ParamT>>{}, std::ref(param)};
	static_assert(sizeof(TriggerParam_t *) == sizeof(LPARAM));
	LPARAM const p = reinterpret_cast<LPARAM>(&wrapper);
	auto const func = header_object.hoAdRunHeader->rh4.rh4KpxFunctions[RFUNCTION_GENERATEEVENT].routine;
	func(&header_object, condition_id, p);
}
static auto get_triggered_condition_param_v(headerObject &header_object) noexcept
-> TriggerParam_t *
{
	if(LPARAM const p = header_object.hoAdRunHeader->rhCurParam[0])
	{
		return reinterpret_cast<TriggerParam_t *>(p);
	}
	return nullptr;
}
template<typename ParamT>
static auto get_triggered_condition_param(headerObject &header_object) noexcept
-> ParamT *
{
	if(TriggerParam_t *wrapper = get_triggered_condition_param_v(header_object))
	{
		if(holds_alternative<std::reference_wrapper<ParamT>>(*wrapper))
		{
			return &(get<std::reference_wrapper<ParamT>>(*wrapper).get());
		}
	}
	return nullptr;
}

template<typename Callable>
static void handle_exceptions(RunData *const run_data, void const *const ace, Callable &&f) noexcept
{
	#ifdef FUSION_RUNTIME_ONLY
	f();
	#else
	try
	{
		f();
	}
	catch(std::bad_alloc const &e)
	{
		static bool already_notified = false;
		if(!already_notified)
		{
			already_notified = true;
			(void)MessageBox(NULL, TSL("Out of memory (std::bad_alloc)"), TSL("Internal List Object"), MB_OK|MB_ICONERROR);
		}
		TriggerOnError param {ace, &e};
		trigger_condition(run_data->rHo, decltype(param)::Id, param);
	}
	catch(std::exception const &e)
	{
		TriggerOnError param {ace, &e};
		trigger_condition(run_data->rHo, decltype(param)::Id, param);
	}
	catch(...)
	{
		TriggerOnError param {ace};
		trigger_condition(run_data->rHo, decltype(param)::Id, param);
	}
	#endif
}

template<typename Container, typename IndexT>
static auto iterator_from_index(Container &container, IndexT const index, bool const underflow_to_last, bool const overflow_to_last) noexcept
{
	using std::empty;
	using std::size;
	using std::begin;
	using std::end;
	if(empty(container))
	{
		return end(container);
	}
	if(index < 0)
	{
		if(underflow_to_last)
		{
			return end(container) - 1;
		}
		return end(container);
	}
	if(static_cast<std::size_t>(index) >= size(container))
	{
		if(overflow_to_last)
		{
			return end(container) - 1;
		}
		return end(container);
	}
	return begin(container) + index;
}
template<typename Container, typename IndexT>
static auto const_iterator_from_index(Container const &container, IndexT const index, bool const underflow_to_last, bool const overflow_to_last) noexcept
{
	return iterator_from_index(container, index, underflow_to_last, overflow_to_last);
}

struct free_via_mv final
{
	mv *mV;
	void operator()(void *p) const noexcept
	{
		mvFree(mV, p);
	}
};
static auto load_text_file(RunData const *const run_data, char_t const *const filename) noexcept
{
	return std::unique_ptr<char_t, free_via_mv>
	{
		mvLoadTextFile(run_data->rHo.hoAdRunHeader->rh4.rh4Mv, filename, CHARENC_DEFAULT, FALSE),
		free_via_mv   {run_data->rHo.hoAdRunHeader->rh4.rh4Mv}
	};
}
static auto save_text_file(RunData const *const run_data, char_t const *const filename, char_t const *const text) noexcept
{
	return mvSaveTextFile(run_data->rHo.hoAdRunHeader->rh4.rh4Mv, filename, text, CHARENC_DEFAULT);
}

static auto alphanum_less(string_view_t a, string_view_t b) noexcept
-> bool
{
	static constexpr string_view_t const numbers = TSL("0123456789");
	while(!empty(a) && !empty(b))
	{
		std::size_t const ani = a.find_first_of(numbers);
		std::size_t const aai = a.find_first_not_of(numbers);
		std::size_t const bni = b.find_first_of(numbers);
		std::size_t const bai = b.find_first_not_of(numbers);
		if(ani == 0 && bni == 0)
		{
			auto const na = std::stoull(string_t{a.substr(0, aai)});
			auto const nb = std::stoull(string_t{b.substr(0, bai)});
			if(na != nb)
			{
				return na < nb;
			}
			a.remove_prefix(std::min(aai, size(a)));
			b.remove_prefix(std::min(bai, size(b)));
		}
		else if(aai == 0 && bai == 0)
		{
			string_view_t const sa = a.substr(0, ani);
			string_view_t const sb = b.substr(0, bni);
			if(sa != sb)
			{
				return sa < sb;
			}
			a.remove_prefix(std::min(ani, size(a)));
			b.remove_prefix(std::min(bni, size(b)));
		}
		else
		{
			break;
		}
	}
	return a < b;
}

#ifdef FUSION_ACTIONS
auto FUSION_API action_insert_item(RunData *const run_data, ac_param_t const item, ac_param_t const i_before) noexcept
-> action_return_t
{
	handle_exceptions(run_data, &action_insert_item, [&]
	{
		Data::list_t &list = run_data->current();
		auto const it_before = const_iterator_from_index(list, i_before, false, false);
		if(it_before == cend(list))
		{
			list.emplace_back(reinterpret_cast<char_t const *>(item));
		}
		else
		{
			list.emplace(it_before, reinterpret_cast<char_t const *>(item));
		}
	});

	return action_return_t();
}
auto FUSION_API action_insert_item_times(RunData *const run_data, ac_param_t const, ac_param_t const) noexcept
-> action_return_t
{
	string_view_t const item = reinterpret_cast<char_t const *>(CNC_GetStringParameter(run_data));
	ac_param_t const i_before = CNC_GetIntParameter(run_data);
	ac_param_t const times = CNC_GetIntParameter(run_data);

	handle_exceptions(run_data, &action_insert_item_times, [&]
	{
		if(times > 0)
		{
			Data::list_t &list = run_data->current();
			list.reserve(size(list) + static_cast<std::size_t>(times));
			auto const it_before = const_iterator_from_index(list, i_before, false, false);
			if(it_before == cend(list))
			{
				for(ac_param_t i = 0; i < times; ++i)
				{
					list.emplace_back(item);
				}
			}
			else
			{
				for(ac_param_t i = 0; i < times; ++i)
				{
					list.emplace(it_before, item);
				}
			}
		}
	});

	return action_return_t();
}

auto FUSION_API action_change_item(RunData *const run_data, ac_param_t const index, ac_param_t const item) noexcept
-> action_return_t
{
	handle_exceptions(run_data, &action_change_item, [&]
	{
		Data::list_t &list = run_data->current();
		auto const it = iterator_from_index(list, index, true, true);
		if(it != end(list))
		{
			it->assign(reinterpret_cast<char_t const *>(item));
		}
	});

	return action_return_t();
}

auto FUSION_API action_copy_items(RunData *const run_data, ac_param_t const, ac_param_t const) noexcept
-> action_return_t
{
	ac_param_t const i_start = CNC_GetIntParameter(run_data);
	ac_param_t const i_end = CNC_GetIntParameter(run_data);
	ac_param_t const i_before = CNC_GetIntParameter(run_data);
	ac_param_t const mode = CNC_GetIntParameter(run_data);

	handle_exceptions(run_data, &action_copy_items, [&]
	{
		Data::list_t &list = run_data->current();
		auto const it_start  = const_iterator_from_index(list, i_start , true , false);
		auto const it_end    = const_iterator_from_index(list, i_end   , false, false);
		auto       it_before =       iterator_from_index(list, i_before, false, false);
		if(it_start >= it_end)
		{
			return;
		}

		Data::list_t temp (it_start, it_end);
		if(mode == 0)
		{
			list.insert(it_before, begin(temp), end(temp));
		}
		else
		{
			for(auto &&v : temp)
			{
				if(it_before == end(list))
				{
					list.emplace_back(std::move(v));
					it_before = end(list);
				}
				else
				{
					*it_before = std::move(v);
					++it_before;
				}
			}
		}
	});

	return action_return_t();
}
auto FUSION_API action_copy_items_from_list(RunData *const run_data, ac_param_t const, ac_param_t const) noexcept
-> action_return_t
{
	string_view_t const list_from_name = reinterpret_cast<char_t const *>(CNC_GetStringParameter(run_data));
	ac_param_t const i_start = CNC_GetIntParameter(run_data);
	ac_param_t const i_end = CNC_GetIntParameter(run_data);
	ac_param_t const i_before = CNC_GetIntParameter(run_data);
	ac_param_t const mode = CNC_GetIntParameter(run_data);

	handle_exceptions(run_data, &action_copy_items_from_list, [&]
	{
		auto const *const list_from_ptr = run_data->list_optional(list_from_name);
		if(!list_from_ptr)
		{
			return;
		}
		Data::list_t const &list_from = *list_from_ptr;
		Data::list_t &list_to = run_data->current();
		auto const it_start  = const_iterator_from_index(list_from, i_start , true , false);
		auto const it_end    = const_iterator_from_index(list_from, i_end   , false, false);
		auto       it_before =       iterator_from_index(list_to  , i_before, false, false);
		if(it_start >= it_end)
		{
			return;
		}

		Data::list_t temp (it_start, it_end);
		if(mode == 0)
		{
			list_to.insert(it_before, begin(temp), end(temp));
		}
		else
		{
			for(auto &&v : temp)
			{
				if(it_before == end(list_to))
				{
					list_to.emplace_back(std::move(v));
					it_before = end(list_to);
				}
				else
				{
					*it_before = std::move(v);
					++it_before;
				}
			}
		}
	});

	return action_return_t();
}

auto FUSION_API action_delete_items(RunData *const run_data, ac_param_t const i_start, ac_param_t const i_end) noexcept
-> action_return_t
{
	handle_exceptions(run_data, &action_delete_items, [&]
	{
		Data::list_t &list = run_data->current();
		auto const it_start = const_iterator_from_index(list, i_start , true , false);
		auto const it_end   = const_iterator_from_index(list, i_end   , false, false);
		if(it_start >= it_end)
		{
			return;
		}

		list.erase(it_start, it_end);
	});

	return action_return_t();
}

auto FUSION_API action_clear_list(RunData *const run_data, ac_param_t const, ac_param_t const) noexcept
-> action_return_t
{
	handle_exceptions(run_data, &action_clear_list, [&]
	{
		run_data->data().lists.erase(*(run_data->current_list_name));
	});

	return action_return_t();
}

auto FUSION_API action_save_to_file(RunData *const run_data, ac_param_t const, ac_param_t const) noexcept
-> action_return_t
{
	auto const filename = reinterpret_cast<char_t const *>(CNC_GetStringParameter(run_data));
	string_view_t const delimiter = reinterpret_cast<char_t const *>(CNC_GetStringParameter(run_data));
	string_view_t const escape = reinterpret_cast<char_t const *>(CNC_GetStringParameter(run_data));

	handle_exceptions(run_data, &action_save_to_file, [&]
	{
		Data::list_t const &list = run_data->current();
		string_t text;
		for(std::size_t i = 0; i < size(list); ++i)
		{
			if(i != 0)
			{
				text.append(delimiter);
			}
			text.append(escape_item(list[i], delimiter, escape));
		}
		save_text_file(run_data, filename, text.c_str());
	});

	return action_return_t();
}

auto FUSION_API action_load_from_file(RunData *const run_data, ac_param_t const, ac_param_t const) noexcept
-> action_return_t
{
	auto const filename = reinterpret_cast<char_t const *>(CNC_GetStringParameter(run_data));
	string_view_t const delimiter = reinterpret_cast<char_t const *>(CNC_GetStringParameter(run_data));
	string_view_t const escape = reinterpret_cast<char_t const *>(CNC_GetStringParameter(run_data));
	ac_param_t const i_before = CNC_GetIntParameter(run_data);

	handle_exceptions(run_data, &action_load_from_file, [&]
	{
		if(auto const text = load_text_file(run_data, filename))
		{
			Data::list_t &list = run_data->current();
			auto it_before = const_iterator_from_index(list, i_before, false, false);
			split_escaped(text.get(), delimiter, escape, [&](string_view_t const item)
			{
				if(it_before == cend(list))
				{
					list.emplace_back(item);
					it_before = cend(list);
				}
				else
				{
					it_before = list.emplace(it_before, item) + 1;
				}
			});
		}
	});

	return action_return_t();
}

auto FUSION_API action_load_from_string(RunData *const run_data, ac_param_t const, ac_param_t const) noexcept
-> action_return_t
{
	string_view_t const source = reinterpret_cast<char_t const *>(CNC_GetStringParameter(run_data));
	string_view_t const delimiter = reinterpret_cast<char_t const *>(CNC_GetStringParameter(run_data));
	string_view_t const escape = reinterpret_cast<char_t const *>(CNC_GetStringParameter(run_data));
	ac_param_t const i_before = CNC_GetIntParameter(run_data);

	handle_exceptions(run_data, &action_load_from_string, [&]
	{
		Data::list_t &list = run_data->current();
		auto it_before = const_iterator_from_index(list, i_before, false, false);
		split_escaped(source, delimiter, escape, [&](string_view_t const item)
		{
			if(it_before == cend(list))
			{
				list.emplace_back(item);
				it_before = cend(list);
			}
			else
			{
				it_before = list.emplace(it_before, item) + 1;
			}
		});
	});

	return action_return_t();
}

auto FUSION_API action_sort_range(RunData *const run_data, ac_param_t const, ac_param_t const) noexcept
-> action_return_t
{
	ac_param_t const i_start = CNC_GetIntParameter(run_data);
	ac_param_t const i_end = CNC_GetIntParameter(run_data);
	ac_param_t const sort_order = CNC_GetIntParameter(run_data);

	handle_exceptions(run_data, &action_sort_range, [&]
	{
		Data::list_t &list = run_data->current();
		auto const it_start = iterator_from_index(list, i_start, false, false);
		auto const it_end   = iterator_from_index(list, i_end  , false, false);
		if(it_start >= it_end || it_end - it_start == 1)
		{
			return;
		}
		switch(sort_order)
		{
			case 0: sort(it_start, it_end); break;
			case 1: sort(it_start, it_end, [](auto const &a, auto const &b) noexcept -> bool { return b < a; }); break;
			case 2: sort(it_start, it_end, [&](string_t const &a, string_t const &b) noexcept
			-> bool
			{
				TriggerOnCustomSortCompare param {a.c_str(), b.c_str()};
				trigger_condition(run_data->rHo, decltype(param)::Id, param);
				return param.result;
			}); break;
			case 3: sort(it_start, it_end, alphanum_less); break;
			case 4: sort(it_start, it_end, [](auto const &a, auto const &b) noexcept -> bool { return alphanum_less(b, a); }); break;
		}
	});

	return action_return_t();
}

auto FUSION_API action_custom_sort_return(RunData *const run_data, ac_param_t const r, ac_param_t const) noexcept
-> action_return_t
{
	if(auto *const param = get_triggered_condition_param<TriggerOnCustomSortCompare>(run_data->rHo))
	{
		param->result = (r != 0);
	}

	return action_return_t();
}

auto FUSION_API action_reserve_space(RunData *const run_data, ac_param_t const param0, ac_param_t const) noexcept
-> action_return_t
{
	handle_exceptions(run_data, &action_reserve_space, [&]
	{
		if(param0 > 0)
		{
			run_data->current().reserve(static_cast<std::size_t>(param0));
		}
	});

	return action_return_t();
}

auto FUSION_API action_regex_erase(RunData *const run_data, ac_param_t const param0, ac_param_t const) noexcept
-> action_return_t
{
	handle_exceptions(run_data, &action_regex_erase, [&]
	{
		std::basic_regex<char_t> const re (reinterpret_cast<char_t const *>(param0));
		Data::list_t &list = run_data->current();
		for(auto it = begin(list); it != end(list); )
		{
			if(regex_search(*it, re))
			{
				it = list.erase(it);
			}
			else
			{
				++it;
			}
		}
	});

	return action_return_t();
}

auto FUSION_API action_change_current_list(RunData *const run_data, ac_param_t const param0, ac_param_t const) noexcept
-> action_return_t
{
	handle_exceptions(run_data, &action_change_current_list, [&]
	{
		run_data->current_list_name->assign(reinterpret_cast<char_t const *>(param0));
	});

	return action_return_t();
}

auto FUSION_API action_delete_list(RunData *const run_data, ac_param_t const param0, ac_param_t const) noexcept
-> action_return_t
{
	handle_exceptions(run_data, &action_delete_list, [&]
	{
		run_data->data().lists.erase(reinterpret_cast<char_t const *>(param0));
	});

	return action_return_t();
}

auto FUSION_API action_use_global_data(RunData *const run_data, ac_param_t const param0, ac_param_t const) noexcept
-> action_return_t
{
	lSDK::string_view_t const global_data_name {reinterpret_cast<char_t const *>(param0)};

	handle_exceptions(run_data, &action_use_global_data, [&]
	{
		if(run_data->global_data_name)
		{
			run_data->global_data_name->assign(global_data_name);
		}
		else
		{
			run_data->global_data_name = new Data::global_t::key_type{global_data_name};
		}
	});

	return action_return_t();
}
auto FUSION_API action_use_local_data(RunData *const run_data, ac_param_t const, ac_param_t const) noexcept
-> action_return_t
{
	delete run_data->global_data_name;
	run_data->global_data_name = nullptr;

	return action_return_t();
}
auto FUSION_API action_copy_local_to_global(RunData *const run_data, ac_param_t const param0, ac_param_t const) noexcept
-> action_return_t
{
	lSDK::string_view_t const global_data_name {reinterpret_cast<char_t const *>(param0)};

	handle_exceptions(run_data, &action_use_global_data, [&]
	{
		auto it = Data::global.find(global_data_name);
		if(it == end(Data::global))
		{
			it = Data::global.emplace(global_data_name, Data::global_t::mapped_type{}).first;
		}
		for(auto const &list : run_data->local_data->lists)
		{
			it->second.lists.insert_or_assign(list.first, list.second);
		}
	});

	return action_return_t();
}
auto FUSION_API action_copy_global_to_local(RunData *const run_data, ac_param_t const param0, ac_param_t const) noexcept
-> action_return_t
{
	lSDK::string_view_t const global_data_name {reinterpret_cast<char_t const *>(param0)};

	handle_exceptions(run_data, &action_use_global_data, [&]
	{
		if(auto const it = Data::global.find(global_data_name); it != end(Data::global))
		{
			for(auto const &list : it->second.lists)
			{
				run_data->local_data->lists.insert_or_assign(list.first, list.second);
			}
		}
	});

	return action_return_t();
}
auto FUSION_API action_delete_global_data(RunData *const run_data, ac_param_t const param0, ac_param_t const) noexcept
-> action_return_t
{
	lSDK::string_view_t const global_data_name {reinterpret_cast<char_t const *>(param0)};

	if(auto const it = Data::global.find(global_data_name); it != end(Data::global))
	{
		Data::global.erase(it);
	}

	return action_return_t();
}
auto FUSION_API action_delete_local_data(RunData *const run_data, ac_param_t const, ac_param_t const) noexcept
-> action_return_t
{
	run_data->local_data->lists.clear();

	return action_return_t();
}
#endif

#ifdef FUSION_CONDITIONS
auto FUSION_API condition_item_in_list(RunData *const run_data, ac_param_t const param0, ac_param_t const) noexcept
-> condition_return_t
{
	if(auto const *const list = run_data->current_optional())
	{
		auto const it = find(cbegin(*list), cend(*list), string_view_t{reinterpret_cast<char_t const *>(param0)});
		return it != cend(*list)? FUSION_CONDITIONS_TRUE : FUSION_CONDITIONS_FALSE;
	}
	return FUSION_CONDITIONS_FALSE;
}
auto FUSION_API condition_regex_in_list(RunData *const run_data, ac_param_t const param0, ac_param_t const) noexcept
-> condition_return_t
{
	if(auto const *const list = run_data->current_optional())
	{
		condition_return_t ret = FUSION_CONDITIONS_FALSE;
		handle_exceptions(run_data, &condition_regex_in_list, [&]
		{
			std::basic_regex<char_t> const re (reinterpret_cast<char_t const *>(param0));
			auto const it = find_if(cbegin(*list), cend(*list), [&](string_t const &v) -> bool { return regex_search(v, re); });
			if(it != cend(*list))
			{
				ret = FUSION_CONDITIONS_TRUE;
			}
		});
		return ret;
	}
	return FUSION_CONDITIONS_FALSE;
}

auto FUSION_API condition_on_trigger(RunData *const, ac_param_t const, ac_param_t const) noexcept
-> condition_return_t
{
	return FUSION_CONDITIONS_TRUE;
}
#endif

#ifdef FUSION_EXPRESSIONS
static auto temp_string(RunData *const run_data, string_view_t const str) noexcept
-> expression_return_t
{
	if(!str.empty())
	{
		LPARAM const buffer_size = static_cast<LPARAM>((str.size()+1)*sizeof(TCHAR));
		if(char_t *p = reinterpret_cast<char_t *>(callRunTimeFunction(run_data, RFUNCTION_GETSTRINGSPACE_EX, 0, buffer_size)))
		{
			*std::copy(cbegin(str), cend(str), p) = TSL('\0');
			return reinterpret_cast<expression_return_t>(p);
		}
	}
	return reinterpret_cast<expression_return_t>(TSL(""));
}

auto FUSION_API expression_item_by_index(RunData *const run_data, e_params_t const params_handle) noexcept
-> expression_return_t
{
	auto const index = CNC_GetFirstExpressionParameter(run_data, params_handle, TYPE_INT);
	run_data->rHo.hoFlags |= HOF_STRING;

	if(auto const *const list = run_data->current_optional())
	{
		auto const it = const_iterator_from_index(*list, index, true, false);
		if(it != cend(*list))
		{
			return reinterpret_cast<expression_return_t>(it->c_str());
		}
	}
	return reinterpret_cast<expression_return_t>(TSL(""));
}

auto FUSION_API expression_number_of_items(RunData *const run_data, e_params_t const) noexcept
-> expression_return_t
{
	if(auto const *const list = run_data->current_optional())
	{
		return static_cast<expression_return_t>(size(*list));
	}
	return 0;
}

auto FUSION_API expression_count_item(RunData *const run_data, e_params_t const params_handle) noexcept
-> expression_return_t
{
	string_view_t const item = reinterpret_cast<char_t const *>(CNC_GetFirstExpressionParameter(run_data, params_handle, TYPE_STRING));

	if(auto const *const list = run_data->current_optional())
	{
		return static_cast<expression_return_t>(count(cbegin(*list), cend(*list), item));
	}
	return 0;
}
auto FUSION_API expression_count_regex(RunData *const run_data, e_params_t const params_handle) noexcept
-> expression_return_t
{
	string_view_t const pattern = reinterpret_cast<char_t const *>(CNC_GetFirstExpressionParameter(run_data, params_handle, TYPE_STRING));

	if(auto const *const list = run_data->current_optional())
	{
		expression_return_t ret = 0;
		handle_exceptions(run_data, &expression_count_regex, [&]
		{
			std::basic_regex<char_t> const re (data(pattern));
			ret = static_cast<expression_return_t>(count_if(cbegin(*list), cend(*list), [&](string_t const &v) -> bool { return regex_search(v, re); }));
		});
		return ret;
	}
	return 0;
}

auto FUSION_API expression_nth_item_index(RunData *const run_data, e_params_t const params_handle) noexcept
-> expression_return_t
{
	string_view_t const item = reinterpret_cast<char_t const *>(CNC_GetFirstExpressionParameter(run_data, params_handle, TYPE_STRING));
	auto n = CNC_GetNextExpressionParameter(run_data, params_handle, TYPE_INT);

	if(n > 0)
	{
		if(auto const *const list = run_data->current_optional())
		{
			for(std::size_t i = 0; i < size(*list); ++i)
			{
				if((*list)[i] == item)
				{
					if(--n == 0)
					{
						return i;
					}
				}
			}
		}
	}
	return -1;
}
auto FUSION_API expression_nth_regex_index(RunData *const run_data, e_params_t const params_handle) noexcept
-> expression_return_t
{
	string_view_t const pattern = reinterpret_cast<char_t const *>(CNC_GetFirstExpressionParameter(run_data, params_handle, TYPE_STRING));
	auto n = CNC_GetNextExpressionParameter(run_data, params_handle, TYPE_INT);

	if(n > 0)
	{
		if(auto const *const list = run_data->current_optional())
		{
			expression_return_t ret = -1;
			handle_exceptions(run_data, &expression_nth_regex_index, [&]
			{
				std::basic_regex<char_t> const re (data(pattern));
				for(std::size_t i = 0; i < size(*list); ++i)
				{
					if(regex_search((*list)[i], re))
					{
						if(--n == 0)
						{
							ret = i;
							break;
						}
					}
				}
			});
			return ret;
		}
	}
	return -1;
}

auto FUSION_API expression_stringify(RunData *const run_data, e_params_t const params_handle) noexcept
-> expression_return_t
{
	string_view_t const delimiter = reinterpret_cast<char_t const *>(CNC_GetFirstExpressionParameter(run_data, params_handle, TYPE_STRING));
	string_view_t const escape    = reinterpret_cast<char_t const *>(CNC_GetNextExpressionParameter (run_data, params_handle, TYPE_STRING));
	run_data->rHo.hoFlags |= HOF_STRING;

	if(auto const *const list = run_data->current_optional())
	{
		expression_return_t ret = reinterpret_cast<expression_return_t>(TSL(""));
		handle_exceptions(run_data, &expression_stringify, [&]
		{
			ret = temp_string(run_data, combine_escaped(run_data->current(), delimiter, escape));
		});
		return ret;
	}
	return reinterpret_cast<expression_return_t>(TSL(""));
}

auto FUSION_API expression_custom_sort_a(RunData *const run_data, e_params_t const) noexcept
-> expression_return_t
{
	run_data->rHo.hoFlags |= HOF_STRING;

	if(auto const *const param = get_triggered_condition_param<TriggerOnCustomSortCompare>(run_data->rHo))
	{
		return reinterpret_cast<expression_return_t>(param->a);
	}
	return reinterpret_cast<expression_return_t>(TSL(""));
}
auto FUSION_API expression_custom_sort_b(RunData *const run_data, e_params_t const) noexcept
-> expression_return_t
{
	run_data->rHo.hoFlags |= HOF_STRING;

	if(auto const *const param = get_triggered_condition_param<TriggerOnCustomSortCompare>(run_data->rHo))
	{
		return reinterpret_cast<expression_return_t>(param->b);
	}
	return reinterpret_cast<expression_return_t>(TSL(""));
}

auto FUSION_API expression_capacity(RunData *const run_data, e_params_t const) noexcept
-> expression_return_t
{
	if(auto const *const list = run_data->current_optional())
	{
		return static_cast<expression_return_t>(list->capacity());
	}
	return 0;
}

auto FUSION_API expression_current_list(RunData *const run_data, e_params_t const) noexcept
-> expression_return_t
{
	run_data->rHo.hoFlags |= HOF_STRING;
	return reinterpret_cast<expression_return_t>(run_data->current_list_name->c_str());
}

auto FUSION_API expression_number_of_lists(RunData *const run_data, e_params_t const) noexcept
-> expression_return_t
{
	return static_cast<expression_return_t>(run_data->data().lists.size());
}

auto FUSION_API expression_list_name_by_index(RunData *const run_data, e_params_t const params_handle) noexcept
-> expression_return_t
{
	auto const index = CNC_GetFirstExpressionParameter(run_data, params_handle, TYPE_INT);
	run_data->rHo.hoFlags |= HOF_STRING;

	if(index >= 0 && index < size(run_data->data().lists))
	{
		auto it = cbegin(run_data->data().lists);
		advance(it, index);
		return reinterpret_cast<expression_return_t>(it->first.c_str());
	}
	return reinterpret_cast<expression_return_t>(TSL(""));
}

auto FUSION_API expression_change_current_list_s(RunData *const run_data, e_params_t const params_handle) noexcept
-> expression_return_t
{
	string_view_t const list_name = reinterpret_cast<char_t const *>(CNC_GetFirstExpressionParameter(run_data, params_handle, TYPE_STRING));
	run_data->rHo.hoFlags |= HOF_STRING;

	handle_exceptions(run_data, &expression_change_current_list_s, [&]
	{
		run_data->current_list_name->assign(list_name);
	});

	return reinterpret_cast<expression_return_t>(TSL(""));
}
auto FUSION_API expression_change_current_list_i(RunData *const run_data, e_params_t const params_handle) noexcept
-> expression_return_t
{
	string_view_t const list_name = reinterpret_cast<char_t const *>(CNC_GetFirstExpressionParameter(run_data, params_handle, TYPE_STRING));

	handle_exceptions(run_data, &expression_change_current_list_i, [&]
	{
		run_data->current_list_name->assign(list_name);
	});

	return 0;
}

auto FUSION_API expression_error_message(RunData *const run_data, e_params_t const) noexcept
-> expression_return_t
{
	run_data->rHo.hoFlags |= HOF_STRING;

	#ifdef FUSION_RUNTIME_ONLY
	return reinterpret_cast<expression_return_t>(TSL(""));
	#else
	if(auto const *const param = get_triggered_condition_param<TriggerOnError>(run_data->rHo))
	{
		if(param->cached_message)
		{
			return reinterpret_cast<expression_return_t>(param->cached_message.get());
		}

		using namespace std::string_view_literals;
		std::string_view func_name = "<unknown function>"sv;
		#define detect_function_name(f) else if(param->ace == f){ func_name = #f##sv; }
		if(false){}
		detect_function_name(action_insert_item_times)
		detect_function_name(action_change_item)
		detect_function_name(action_copy_items)
		detect_function_name(action_delete_items)
		detect_function_name(action_clear_list)
		detect_function_name(action_save_to_file)
		detect_function_name(action_load_from_file)
		detect_function_name(action_load_from_string)
		detect_function_name(action_sort_range)
		detect_function_name(action_custom_sort_return)
		detect_function_name(action_reserve_space)
		detect_function_name(action_regex_erase)
		detect_function_name(action_change_current_list)
		detect_function_name(action_delete_list)
		detect_function_name(action_copy_items_from_list)
		detect_function_name(action_insert_item)
		detect_function_name(action_use_global_data)
		detect_function_name(action_use_local_data)
		detect_function_name(action_copy_local_to_global)
		detect_function_name(action_copy_global_to_local)
		detect_function_name(action_delete_global_data)
		detect_function_name(action_delete_local_data)
		detect_function_name(condition_item_in_list)
		detect_function_name(condition_regex_in_list)
		detect_function_name(expression_item_by_index)
		detect_function_name(expression_number_of_items)
		detect_function_name(expression_count_item)
		detect_function_name(expression_nth_item_index)
		detect_function_name(expression_stringify)
		detect_function_name(expression_custom_sort_a)
		detect_function_name(expression_custom_sort_b)
		detect_function_name(expression_capacity)
		detect_function_name(expression_count_regex)
		detect_function_name(expression_nth_regex_index)
		detect_function_name(expression_current_list)
		detect_function_name(expression_number_of_lists)
		detect_function_name(expression_list_name_by_index)
		detect_function_name(expression_change_current_list_s)
		detect_function_name(expression_change_current_list_i)
		#undef detect_function_name

		std::string_view const what = (param->e? param->e->what() : "<unknown error>"sv);

		static constexpr std::string_view const part_a = "Error during "sv;
		static constexpr std::string_view const part_b = ": "sv;
		std::size_t const len = size(part_a) + size(func_name) + size(part_b) + size(what);

		try
		{
			param->cached_message = std::make_unique<char_t[]>(len + 1);
		}
		catch(...)
		{
			param->cached_message.reset();
			return reinterpret_cast<expression_return_t>(TSL("<not enough memory for generating the error message>"));
		}

		auto p = std::copy(cbegin(part_a), cend(part_a), param->cached_message.get());
		p = std::copy(cbegin(func_name), cend(func_name), p);
		p = std::copy(cbegin(part_b), cend(part_b), p);
		p = std::copy(cbegin(what), cend(what), p);
		*p = TSL('\0');
		return reinterpret_cast<expression_return_t>(param->cached_message.get());
	}
	return reinterpret_cast<expression_return_t>(TSL("No error currently - make sure you use this expression during the On Error event."));
	#endif
}
#endif
