#include "FusionAPI.hpp"
#include "lSDK.hpp"
#include "lSDK/WindowsUtilities.hpp"
#include "EditData.hpp"
#include "Resources.h"

#include <cassert>

#ifdef FUSION_GET_EXTENSION_METADATA
void FUSION_API GetObjInfos(mv *const mV, SerializedEditData *serialized_edit_data,
	fusion::string_buffer const extension_name_buffer/*255*/,
	fusion::string_buffer const extension_author_buffer/*255*/,
	fusion::string_buffer const extension_copyright_buffer/*255*/,
	fusion::string_buffer const extension_description_buffer/*1024*/,
	fusion::string_buffer const extension_website_buffer/*255*/
) noexcept
{
	lSDK::fill_buffer(extension_name_buffer       , 255 , lSDK::get_resource_string(RILO_NAME));
	lSDK::fill_buffer(extension_author_buffer     , 255 , lSDK::get_resource_string(RILO_AUTHOR));
	lSDK::fill_buffer(extension_copyright_buffer  , 255 , lSDK::get_resource_string(RILO_COPYRIGHT));
	lSDK::fill_buffer(extension_description_buffer, 1024, lSDK::get_resource_string(RILO_DESCRIPTION));
	lSDK::fill_buffer(extension_website_buffer    , 255 , lSDK::get_resource_string(RILO_WEBSITE));
}
#endif

#ifdef FUSION_GET_HELP_FILE
auto FUSION_API GetHelpFileName() noexcept
-> fusion::string_view
{
	static lSDK::string_t help_path;
	help_path.assign(lSDK::get_resource_string(RILO_HELP));
	return help_path.c_str();
}
#endif

#ifndef FUSION_RUNTIME_ONLY
void SerializedEditData::serialize(mv *const mV, SerializedEditData *&serialized_edit_data, Data const &d)
{
	std::size_t new_size = sizeof(std::uint32_t);
	for(auto const &list : d.lists)
	{
		new_size += sizeof(std::uint32_t);
		new_size += size(list.first)*sizeof(lSDK::char_t);
		new_size += sizeof(std::uint32_t);
		for(auto const &item : list.second)
		{
			new_size += sizeof(std::uint32_t);
			new_size += size(item)*sizeof(lSDK::char_t);
		}
	}
	if(auto const new_sed = static_cast<SerializedEditData *>(mvReAllocEditData(mV, serialized_edit_data, static_cast<DWORD>(sizeof(SerializedEditData) + new_size))))
	{
		assert(new_sed->eHeader.extSize == sizeof(SerializedEditData) + new_size);
		serialized_edit_data = new_sed;
		std::byte *p = serialized_edit_data->raw;
		auto const write_len = [&](std::size_t const len) noexcept
		{
			std::uint32_t const v = static_cast<std::uint32_t>(len);
			std::memcpy(p, &v, sizeof(v));
			p += sizeof(v);
		};
		auto const write_string = [&](lSDK::string_view_t const str) noexcept
		{
			write_len(size(str));
			std::memcpy(p, data(str), size(str)*sizeof(lSDK::char_t));
			p += size(str)*sizeof(lSDK::char_t);
		};
		write_len(size(d.lists));
		for(auto const &list : d.lists)
		{
			write_string(list.first);
			write_len(size(list.second));
			for(auto const &item : list.second)
			{
				write_string(item);
			}
		}
		assert(p - serialized_edit_data->raw == new_size);
		serialized_edit_data->eHeader.extVersion = CURRENT_VERSION;
	}
	else
	{
		throw std::bad_alloc{};
	}
}
#endif
auto SerializedEditData::deserialize() const
-> Data
{
	Data d;
	auto const split_old_version = [&](lSDK::string_view_t const lists, lSDK::string_view_t const delim_list, lSDK::string_view_t const delim_item, lSDK::string_view_t const escape)
	{
		if(empty(delim_item) || empty(lists))
		{
			return;
		}
		if(!empty(delim_list))
		{
			Data::list_t *list = nullptr;
			split_escaped(lists, delim_list, escape, [&](lSDK::string_view_t const v)
			{
				if(list == nullptr)
				{
					list = &d.lists[lSDK::string_t{v}];
				}
				else
				{
					split_escaped(v, delim_item, escape, [&](lSDK::string_view_t const e){ list->emplace_back(e); });
					list = nullptr;
				}
			});
		}
		else
		{
			Data::list_t &list = d.lists[{}];
			split_escaped(lists, delim_item, escape, [&](lSDK::string_view_t const e){ list.emplace_back(e); });
		}
	};
	switch(eHeader.extVersion)
	{
		case 1: //version before properties were added
		{
		} break;
		case 2: //version w/ properties, before Jamie's request
		{
			struct EDv2 final
			{
				extHeader eHeader;
				std::uint32_t offset_delim_list, offset_delim_item, offset_escape;
				std::byte raw[1];
			} const *v2 {reinterpret_cast<EDv2 const *>(this)};
			lSDK::string_view8_t const lists = reinterpret_cast<lSDK::string_view8_t::value_type const *>(v2->raw);
			lSDK::string_view8_t const delim_list = reinterpret_cast<lSDK::string_view8_t::value_type const *>(v2->raw + v2->offset_delim_list);
			lSDK::string_view8_t const delim_item = reinterpret_cast<lSDK::string_view8_t::value_type const *>(v2->raw + v2->offset_delim_item);
			lSDK::string_view8_t const escape = reinterpret_cast<lSDK::string_view8_t::value_type const *>(v2->raw + v2->offset_escape);
			split_old_version(lSDK::string_t_from(lists), lSDK::string_t_from(delim_list), lSDK::string_t_from(delim_item), lSDK::string_t_from(escape));
		} break;
		case 3: //last ANSI version
		{
			struct EDv3 final
			{
				extHeader eHeader;
				std::byte raw[1];
			} const *v3 {reinterpret_cast<EDv3 const *>(this)};
			lSDK::string_view8_t const lists_1 = reinterpret_cast<lSDK::string_view8_t::value_type const *>(v3->raw);
			lSDK::string_view8_t const lists_2 = data(lists_1) + size(lists_1) + 1;
			lSDK::string_view8_t const delim_list = data(lists_2) + size(lists_2) + 1;
			lSDK::string_view8_t const delim_item = data(delim_list) + size(delim_list) + 1;
			lSDK::string_view8_t const escape = data(delim_item) + size(delim_item) + 1;
			split_old_version(lSDK::string_t_from(lists_1) + lSDK::string_t_from(delim_list) + lSDK::string_t_from(lists_2), lSDK::string_t_from(delim_list), lSDK::string_t_from(delim_item), lSDK::string_t_from(escape));
		} break;
		case 4: //current version
		{
			if(eHeader.extSize < sizeof(SerializedEditData))
			{
				return d;
			}

			std::byte const *p = raw;
			auto const read_len = [&]() noexcept
			-> std::size_t
			{
				std::uint32_t v {};
				std::memcpy(&v, p, sizeof(v));
				p += sizeof(v);
				assert(p - raw <= eHeader.extSize - offsetof(SerializedEditData, raw));
				return v;
			};
			auto const read_string = [&]()
			-> lSDK::string_t
			{
				std::size_t const len = read_len();
				lSDK::string_t str (len, TSL('\0'));
				std::memcpy(data(str), p, len*sizeof(lSDK::char_t));
				p += len*sizeof(lSDK::char_t);
				assert(p - raw <= eHeader.extSize - offsetof(SerializedEditData, raw));
				return str;
			};

			std::size_t const num_lists = read_len();
			for(std::size_t i_list = 0; i_list < num_lists; ++i_list)
			{
				lSDK::string_t list_name = read_string();
				std::size_t const num_items = read_len();
				Data::list_t list;
				list.reserve(num_items);
				for(std::size_t i_item = 0; i_item < num_items; ++i_item)
				{
					list.emplace_back(read_string());
				}
				d.lists.emplace(std::move(list_name), std::move(list));
			}
		} break;
		default:
		{
			(void)MessageBox(NULL, _T("This MFA was saved with an unkown version of the Internal List Object. Properties will be reset.\nNote that some actions, conditions, or expressions may not be valid and can cause Fusion to crash."), _T("Warning - Internal List Object"), MB_OK);
		} break;
	}
	return d;
}
