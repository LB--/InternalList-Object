#include "FusionAPI.hpp"
#include "lSDK.hpp"
#include "lSDK/FusionMenu.hpp"
#include "lSDK/FusionUtilities.hpp"
#include "Resources.h"

#include <cassert>

#ifdef FUSION_GET_ACTION_MENU
auto FUSION_API GetActionMenu(mv *const mV, OI *const object_info, SerializedEditData *serialized_edit_data) noexcept
-> HMENU
{
	using Menu = lSDK::ActionMenu;
	return Menu
	{{
		Menu::Separator{},
		Menu::Item{35, RILOAM_INSERT_ITEM},
		Menu::Item{0, RILOAM_INSERT_ITEM_TIMES},
		Menu::Item{1, RILOAM_CHANGE_ITEM},
		Menu::Separator{},
		Menu::Item{2, RILOAM_COPY_ITEMS},
		Menu::Item{14, RILOAM_COPY_FROM_LIST},
		Menu::Separator{},
		Menu::Item{3, RILOAM_DELETE_ITEMS},
		Menu::Item{11, RILOAM_DELETE_ITEMS_REGEX},
		Menu::Item{4, RILOAM_CLEAR},
		Menu::Item{10, RILOAM_RESERVE},
		Menu::Separator{},
		Menu::Item{5, RILOAM_SAVE_FILE},
		Menu::Item{6, RILOAM_LOAD_FILE},
		Menu::Item{7, RILOAM_LOAD_STRING},
		Menu::Separator{},
		Menu::Item{8, RILOAM_SORT},
		Menu::Item{9, RILOAM_SORT_CUSTOM_RESULT},
		Menu::Separator{},
		Menu::Item{12, RILOAM_CHANGE_CURRENT_LIST},
		Menu::Item{13, RILOAM_DELETE_LIST},
		Menu::Separator{},
		Menu::Item{36, RILOAM_USE_GLOBAL_DATA},
		Menu::Item{37, RILOAM_USE_LOCAL_DATA},
		Menu::Item{38, RILOAM_COPY_LOCAL_TO_GLOBAL},
		Menu::Item{39, RILOAM_COPY_GLOBAL_TO_LOCAL},
		Menu::Item{40, RILOAM_DELETE_GLOBAL_DATA},
		Menu::Item{41, RILOAM_DELETE_LOCAL_DATA},
		Menu::Separator{}
	}}.generate().release();
}
#endif
#ifdef FUSION_GET_CONDITION_MENU
auto FUSION_API GetConditionMenu(mv *const mV, OI *const object_info, SerializedEditData *serialized_edit_data) noexcept
-> HMENU
{
	using Menu = lSDK::ConditionMenu;
	return Menu
	{{
		Menu::Separator{},
		Menu::Item{2, RILOCM_ON_ERROR},
		Menu::Item{1, RILOCM_ON_SORT_CUSTOM},
		Menu::Separator{},
		Menu::Item{0, RILOCM_HAS_ITEM},
		Menu::Item{3, RILOCM_HAS_ITEM_REGEX},
		Menu::Separator{}
	}}.generate().release();
}
#endif
#ifdef FUSION_GET_EXPRESSION_MENU
auto FUSION_API GetExpressionMenu(mv *const mV, OI *const object_info, SerializedEditData *serialized_edit_data) noexcept
-> HMENU
{
	using Menu = lSDK::ExpressionMenu;
	return Menu
	{{
		Menu::Separator{},
		Menu::Item{0, RILOEM_GET_ITEM},
		Menu::Item{1, RILOEM_GET_LIST_SIZE},
		Menu::Separator{},
		Menu::Item{2, RILOEM_COUNT_ITEM},
		Menu::Item{3, RILOEM_COUNT_ITEM_INDEX},
		Menu::Item{9, RILOEM_COUNT_ITEM_REGEX},
		Menu::Item{10, RILOEM_COUNT_ITEM_REGEX_INDEX},
		Menu::Separator{},
		Menu::Item{4, RILOEM_SERIALIZE_LIST},
		Menu::Separator{},
		Menu::Item{5, RILOEM_SORT_CUSTOM_A},
		Menu::Item{6, RILOEM_SORT_CUSTOM_B},
		Menu::Separator{},
		Menu::Item{7, RILOEM_CAPACITY},
		Menu::Separator{},
		Menu::Item{11, RILOEM_LISTS_CURRENT},
		Menu::Item{12, RILOEM_LISTS_COUNT},
		Menu::Item{13, RILOEM_LISTS_BY_INDEX},
		Menu::Separator{},
		Menu::Item{14, RILOEM_LISTS_CHANGE_STR},
		Menu::Item{15, RILOEM_LISTS_CHANGE_NUM},
		Menu::Separator{},
		Menu::Item{25, RILOEM_ERROR_MESSAGE},
		Menu::Separator{}
	}}.generate().release();
}
#endif

#ifdef FUSION_GET_ACTION_ID_FROM_ACTION_MENU_ID
auto FUSION_API GetActionCodeFromMenu(mv *const mV, std::int16_t const action_menu_id) noexcept
-> std::int16_t
{
	return lSDK::ActionMenu::ExtensionData::from_menu_id(action_menu_id);
}
#endif
#ifdef FUSION_GET_CONDITION_ID_FROM_CONDITION_MENU_ID
auto FUSION_API GetConditionCodeFromMenu(mv *const mV, std::int16_t const condition_menu_id) noexcept
-> std::int16_t
{
	return lSDK::ConditionMenu::ExtensionData::from_menu_id(condition_menu_id);
}
#endif
#ifdef FUSION_GET_EXPRESSION_ID_FROM_EXPRESSION_MENU_ID
auto FUSION_API GetExpressionCodeFromMenu(mv *const mV, std::int16_t const expression_menu_id) noexcept
-> std::int16_t
{
	return lSDK::ExpressionMenu::ExtensionData::from_menu_id(expression_menu_id);
}
#endif

#ifdef FUSION_GET_ACTION_INFO_ARRAY
auto FUSION_API GetActionInfos(mv *const mV, std::int16_t const action_id) noexcept
-> fusion::ace_info_array
{
	switch(action_id)
	{
		case 0: return lSDK::constant_ace_info<0, 0, PARAM_EXPSTRING, PARAM_EXPRESSION, PARAM_EXPRESSION>();
		case 1: return lSDK::constant_ace_info<1, 0, PARAM_EXPRESSION, PARAM_EXPSTRING>();
		case 2: return lSDK::constant_ace_info<2, 0, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION>();
		case 3: return lSDK::constant_ace_info<3, 0, PARAM_EXPRESSION, PARAM_EXPRESSION>();
		case 4: return lSDK::constant_ace_info<4, 0>();
		case 5: return lSDK::constant_ace_info<5, 0, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPSTRING>();
		case 6: return lSDK::constant_ace_info<6, 0, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPRESSION>();
		case 7: return lSDK::constant_ace_info<7, 0, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPRESSION>();
		case 8: return lSDK::constant_ace_info<8, 0, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION>();
		case 9: return lSDK::constant_ace_info<9, 0, PARAM_EXPRESSION>();
		case 10: return lSDK::constant_ace_info<10, 0, PARAM_EXPRESSION>();
		case 11: return lSDK::constant_ace_info<11, 0, PARAM_EXPSTRING>();
		case 12: return lSDK::constant_ace_info<12, 0, PARAM_EXPSTRING>();
		case 13: return lSDK::constant_ace_info<13, 0, PARAM_EXPSTRING>();
		case 14: return lSDK::constant_ace_info<14, 0, PARAM_EXPSTRING, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION>();
		case 15: return lSDK::constant_ace_info<15, 0, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPRESSION>();
		case 16: return lSDK::constant_ace_info<16, 0>();
		case 17: return lSDK::constant_ace_info<17, 0>();
		case 18: return lSDK::constant_ace_info<18, 0>();
		case 19: return lSDK::constant_ace_info<19, 0>();
		case 20: return lSDK::constant_ace_info<20, 0, PARAM_EXPSTRING, PARAM_EXPRESSION>();
		case 21: return lSDK::constant_ace_info<21, 0, PARAM_EXPSTRING>();
		case 22: return lSDK::constant_ace_info<22, 0, PARAM_EXPSTRING, PARAM_EXPSTRING>();
		case 23: return lSDK::constant_ace_info<23, 0, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPSTRING>();
		case 24: return lSDK::constant_ace_info<24, 0, PARAM_EXPRESSION, PARAM_EXPRESSION>();
		case 25: return lSDK::constant_ace_info<25, 0, PARAM_EXPSTRING, PARAM_EXPSTRING>();
		case 26: return lSDK::constant_ace_info<26, 0>();
		case 27: return lSDK::constant_ace_info<27, 0, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPSTRING>();
		case 28: return lSDK::constant_ace_info<28, 0, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPSTRING>();
		case 29: return lSDK::constant_ace_info<29, 0, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPSTRING>();
		case 30: return lSDK::constant_ace_info<30, 0, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPRESSION>();
		case 31: return lSDK::constant_ace_info<31, 0, PARAM_EXPSTRING>();
		case 32: return lSDK::constant_ace_info<32, 0, PARAM_EXPSTRING>();
		case 33: return lSDK::constant_ace_info<33, 0, PARAM_EXPSTRING>();
		case 34: return lSDK::constant_ace_info<34, 0, PARAM_EXPSTRING>();
		case 35: return lSDK::constant_ace_info<35, 0, PARAM_EXPSTRING, PARAM_EXPRESSION>();
		case 36: return lSDK::constant_ace_info<36, 0, PARAM_EXPSTRING>();
		case 37: return lSDK::constant_ace_info<37, 0>();
		case 38: return lSDK::constant_ace_info<38, 0, PARAM_EXPSTRING>();
		case 39: return lSDK::constant_ace_info<39, 0, PARAM_EXPSTRING>();
		case 40: return lSDK::constant_ace_info<40, 0, PARAM_EXPSTRING>();
		case 41: return lSDK::constant_ace_info<41, 0>();
	}
	return nullptr;
}
#endif
#ifdef FUSION_GET_CONDITION_INFO_ARRAY
auto FUSION_API GetConditionInfos(mv *const mV, std::int16_t const condition_id) noexcept
-> fusion::ace_info_array
{
	switch(condition_id)
	{
		case 0: return lSDK::constant_ace_info<0, EVFLAGS_NOTABLE|EVFLAGS_ALWAYS, PARAM_EXPSTRING>();
		case 1: return lSDK::constant_ace_info<1, EVFLAGS_NOTABLE>();
		case 2: return lSDK::constant_ace_info<2, EVFLAGS_NOTABLE>();
		case 3: return lSDK::constant_ace_info<3, EVFLAGS_NOTABLE|EVFLAGS_ALWAYS, PARAM_EXPSTRING>();
		case 4: return lSDK::constant_ace_info<4, EVFLAGS_NOTABLE|EVFLAGS_ALWAYS>();
		case 5: return lSDK::constant_ace_info<5, EVFLAGS_NOTABLE|EVFLAGS_ALWAYS>();
	}
	return nullptr;
}
#endif
#ifdef FUSION_GET_EXPRESSION_INFO_ARRAY
auto FUSION_API GetExpressionInfos(mv *const mV, std::int16_t const expression_id) noexcept
-> fusion::ace_info_array
{
	switch(expression_id)
	{
		case 0: return lSDK::constant_ace_info<0, EXPFLAG_STRING, EXPPARAM_LONG>();
		case 1: return lSDK::constant_ace_info<1, 0>();
		case 2: return lSDK::constant_ace_info<2, 0, EXPPARAM_STRING>();
		case 3: return lSDK::constant_ace_info<3, 0, EXPPARAM_STRING, EXPPARAM_LONG>();
		case 4: return lSDK::constant_ace_info<4, EXPFLAG_STRING, EXPPARAM_STRING, EXPPARAM_STRING>();
		case 5: return lSDK::constant_ace_info<5, EXPFLAG_STRING>();
		case 6: return lSDK::constant_ace_info<6, EXPFLAG_STRING>();
		case 7: return lSDK::constant_ace_info<7, 0>();
		case 8: return lSDK::constant_ace_info<8, 0>();
		case 9: return lSDK::constant_ace_info<9, 0, EXPPARAM_STRING>();
		case 10: return lSDK::constant_ace_info<10, 0, EXPPARAM_STRING, EXPPARAM_LONG>();
		case 11: return lSDK::constant_ace_info<11, EXPFLAG_STRING>();
		case 12: return lSDK::constant_ace_info<12, 0>();
		case 13: return lSDK::constant_ace_info<13, EXPFLAG_STRING, EXPPARAM_LONG>();
		case 14: return lSDK::constant_ace_info<14, EXPFLAG_STRING, EXPPARAM_STRING>();
		case 15: return lSDK::constant_ace_info<15, 0, EXPPARAM_STRING>();
		case 16: return lSDK::constant_ace_info<16, EXPFLAG_STRING>();
		case 17: return lSDK::constant_ace_info<17, 0>();
		case 18: return lSDK::constant_ace_info<18, EXPFLAG_STRING, EXPPARAM_STRING, EXPPARAM_STRING>();
		case 19: return lSDK::constant_ace_info<19, EXPFLAG_STRING>();
		case 20: return lSDK::constant_ace_info<20, EXPFLAG_STRING>();
		case 21: return lSDK::constant_ace_info<21, 0>();
		case 22: return lSDK::constant_ace_info<22, EXPFLAG_STRING, EXPPARAM_LONG>();
		case 23: return lSDK::constant_ace_info<23, EXPFLAG_STRING, EXPPARAM_STRING>();
		case 24: return lSDK::constant_ace_info<24, 0, EXPPARAM_STRING>();
		case 25: return lSDK::constant_ace_info<25, EXPFLAG_STRING>();
	}
	return nullptr;
}
#endif

#ifdef FUSION_GET_ACTION_PARAM_NAME
void FUSION_API GetActionTitle(mv *const mV, std::int16_t const action_id, std::int16_t const param_id, fusion::string_buffer const param_name_buffer, std::int16_t const param_name_buffer_size) noexcept
{
	auto const f = [=](std::initializer_list<::DWORD> const params) noexcept
	{
		assert(param_id < params.size());
		return (void)lSDK::fill_buffer(param_name_buffer, param_name_buffer_size, lSDK::get_resource_string(*(params.begin() + param_id)));
	};
	switch(action_id)
	{
		case 0: return f({RILOP_ITEM, RILOP_INDEX_INSERT_WRAP, RILOP_INSERT_TIMES});
		case 1: return f({RILOP_INDEX, RILOP_ITEM});
		case 2: return f({RILOP_INDEX_START, RILOP_INDEX_END_WRAP, RILOP_INDEX_COPY_BEFORE, RILOP_COPY_MODE});
		case 3: return f({RILOP_INDEX_START, RILOP_INDEX_END_WRAP});
		case 5: return f({RILOP_FILEPATH, RILOP_DELIMITER, RILOP_ESCAPE});
		case 6: return f({RILOP_FILEPATH, RILOP_DELIMITER, RILOP_ESCAPE, RILOP_INDEX_INSERT_WRAP});
		case 7: return f({RILOP_LISTSTRING, RILOP_DELIMITER, RILOP_ESCAPE, RILOP_INDEX_INSERT_WRAP});
		case 8: return f({RILOP_INDEX_START, RILOP_INDEX_END_WRAP, RILOP_SORT_ORDER});
		case 9: return f({RILOP_SORT_ORDER_CUSTOM_RESULT});
		case 10: return f({RILOP_RESERVE_CAPACITY});
		case 11: return f({RILOP_REGEX});
		case 12: return f({RILOP_LISTNAME});
		case 13: return f({RILOP_LISTNAME});
		case 14: return f({RILOP_LISTNAME, RILOP_INDEX_START, RILOP_INDEX_END_WRAP, RILOP_INDEX_COPY_BEFORE, RILOP_COPY_MODE});
		case 35: return f({RILOP_ITEM, RILOP_INDEX_INSERT_WRAP});
		case 36: return f({RILOP_GLOBAL_DATA_NAME});
		case 38: return f({RILOP_GLOBAL_DATA_NAME});
		case 39: return f({RILOP_GLOBAL_DATA_NAME});
		case 40: return f({RILOP_GLOBAL_DATA_NAME});

		case 15: case 20: case 21: case 22: case 23:
		case 24: case 25: case 27: case 28: case 29:
		case 30: case 31: case 32: case 33: case 34:
			return (void)lSDK::fill_buffer(param_name_buffer, param_name_buffer_size, TSL("deprecated, please remove/replace"));
	}
}
#endif
#ifdef FUSION_GET_CONDITION_PARAM_NAME
void FUSION_API GetConditionTitle(mv *const mV, std::int16_t const condition_id, std::int16_t const param_id, fusion::string_buffer const param_name_buffer, std::int16_t const param_name_buffer_size) noexcept
{
	switch(condition_id)
	{
		case 0: return (void)lSDK::fill_buffer(param_name_buffer, param_name_buffer_size, lSDK::get_resource_string(RILOP_ITEM));
		case 3: return (void)lSDK::fill_buffer(param_name_buffer, param_name_buffer_size, lSDK::get_resource_string(RILOP_REGEX));
	}
}
#endif
#ifdef FUSION_GET_EXPRESSION_PARAM_NAME
void FUSION_API GetExpressionParam(mv *const mV, std::int16_t const expression_id, std::int16_t const param_id, fusion::string_buffer const param_name_buffer, std::int16_t const param_name_buffer_size) noexcept
{
	auto const f = [=](std::initializer_list<::DWORD> const params) noexcept
	{
		assert(param_id < params.size());
		return (void)lSDK::fill_buffer(param_name_buffer, param_name_buffer_size, lSDK::get_resource_string(*(params.begin() + param_id)));
	};
	switch(expression_id)
	{
		case 0: return f({RILOP_INDEX});
		case 2: return f({RILOP_ITEM});
		case 3: return f({RILOP_ITEM, RILOP_OCCURRENCE});
		case 4: return f({RILOP_DELIMITER, RILOP_ESCAPE});
		case 9: return f({RILOP_REGEX});
		case 10: return f({RILOP_REGEX, RILOP_OCCURRENCE});
		case 13: return f({RILOP_INDEX});
		case 14: return f({RILOP_LISTNAME});
		case 15: return f({RILOP_LISTNAME});
	}
}
#endif

#ifdef FUSION_GET_ACTION_DISPLAY_STRING
void FUSION_API GetActionString(mv *const mV, std::int16_t const action_id, fusion::string_buffer const display_string_buffer, std::int16_t const display_string_buffer_size) noexcept
{
	switch(action_id)
	{
		case 0: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_INSERT_TIMES));
		case 1: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_SET));
		case 2: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_COPY));
		case 3: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_ERASE));
		case 4: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_CLEAR));
		case 5: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_SAVE_FILE));
		case 6: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_LOAD_FILE));
		case 7: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_LOAD_STRING));
		case 8: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_SORT));
		case 9: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_SORT_CUSTOM_RESULT));
		case 10: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_RESERVE_CAPACITY));
		case 11: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_ERASE_REGEX));
		case 12: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_CHANGE_CURRENT_LIST));
		case 13: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_DELETE_LIST));
		case 14: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_COPY_FROM_LIST));
		case 15: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Copy Items from current Linked List in range [%0, %1) to %2"));
		case 16: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Iterate Forward"));
		case 17: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Iterate Backward"));
		case 18: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Iterate to Beginning"));
		case 19: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Iterate to End"));
		case 20: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Insert Item %0, %1 times"));
		case 21: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Change Item to %0"));
		case 22: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Copy Items from range [%0, %1)"));
		case 23: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Copy Items from Linked List %0, range [%1, %2)"));
		case 24: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Copy Items from current Contiguous List, range [%0, %1)"));
		case 25: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Delete Items in range [%0, %1)"));
		case 26: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Clear List"));
		case 27: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Save List to File %0 with item Delimiter %1 and Escape with %2"));
		case 28: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Load List from File %0 with item Delimiter %1 and Escape with %2"));
		case 29: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Load List from String %0 with item Delimiter %1 and Escape with %2"));
		case 30: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Sort List in range [%0, %1) with Sort Order %2"));
		case 31: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Change editing list to %0"));
		case 32: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Change editing iterator to %0"));
		case 33: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Delete list %0"));
		case 34: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[LL REMOVED, PLEASE REPLACE] Delete iterator %0"));
		case 35: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_INSERT));
		case 36: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_USE_GLOBAL_DATA));
		case 37: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_USE_LOCAL_DATA));
		case 38: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_COPY_LOCAL_TO_GLOBAL));
		case 39: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_COPY_GLOBAL_TO_LOCAL));
		case 40: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_DELETE_GLOBAL_DATA));
		case 41: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOA_DELETE_LOCAL_DATA));
	}
}
#endif
#ifdef FUSION_GET_CONDITION_DISPLAY_STRING
void FUSION_API GetConditionString(mv *const mV, std::int16_t const condition_id, fusion::string_buffer const display_string_buffer, std::int16_t const display_string_buffer_size) noexcept
{
	switch(condition_id)
	{
		case 0: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOC_CONTAINS));
		case 1: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOC_SORT_CUSTOM));
		case 2: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOC_ERROR));
		case 3: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, lSDK::get_resource_string(RILOC_CONTAINS_REGEX));
		case 4: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[REMOVED, PLEASE REPLACE] %o: Iterator is at beginning"));
		case 5: return (void)lSDK::fill_buffer(display_string_buffer, display_string_buffer_size, TSL("[REMOVED, PLEASE REPLACE] %o: Iterator is at end"));
	}
}
#endif

#ifdef FUSION_GET_EXPRESSION_EDITOR_STRING
void FUSION_API GetExpressionString(mv *const mV, std::int16_t const expression_id, fusion::string_buffer const editor_string_buffer, std::int16_t const editor_string_buffer_size) noexcept
{
	switch(expression_id)
	{
		case 0: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_ITEM));
		case 1: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_SIZE));
		case 2: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_COUNT));
		case 3: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_COUNT_INDEX));
		case 4: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_SERIALIZE));
		case 5: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_SORT_CUSTOM_A));
		case 6: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_SORT_CUSTOM_B));
		case 7: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_CAPACITY));
		case 8: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, TSL("OLD!UNUSED("));
		case 9: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_COUNT_REGEX));
		case 10: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_COUNT_REGEX_INDEX));
		case 11: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_LISTS_CURRENT));
		case 12: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_LISTS_COUNT));
		case 13: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_LISTS_BY_INDEX));
		case 14: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_LISTS_CHANGE_STR));
		case 15: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_LISTS_CHANGE_NUM));
		case 16: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, TSL("OLD!LLItem$("));
		case 17: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, TSL("OLD!LLItems("));
		case 18: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, TSL("OLD!LList$("));
		case 19: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, TSL("OLD!CurrentLList$("));
		case 20: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, TSL("OLD!CurrentLLIterator$("));
		case 21: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, TSL("OLD!LLists("));
		case 22: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, TSL("OLD!LListByIndex$("));
		case 23: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, TSL("OLD!ChangeLList$("));
		case 24: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, TSL("OLD!ChangeLList("));
		case 25: return (void)lSDK::fill_buffer(editor_string_buffer, editor_string_buffer_size, lSDK::get_resource_string(RILOE_ERROR_MESSAGE));
	}
}
#endif
