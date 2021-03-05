#include "FusionAPI.hpp"
#include "lSDK.hpp"
#include "lSDK/WindowsUtilities.hpp"
#include "EditData.hpp"
#include "Resources.h"

#include <array>
#include <cassert>
#include <unordered_map>
#include <vector>

#ifndef FUSION_RUNTIME_ONLY
struct PropsApi final
{
	mv *mV;
	SerializedEditData *&serialized_edit_data;

	void insert(std::vector<PropData> const &props, std::int32_t near_id, bool after) noexcept
	{
		assert(!empty(props) && props.back().dwID == 0);
		mvInsertProps(mV, serialized_edit_data, const_cast<PropData *>(data(props)), static_cast<UINT>(near_id), after? TRUE : FALSE);
	}
	void refresh(std::int32_t prop_id, bool reinit_create_param) noexcept
	{
		mvRefreshProp(mV, serialized_edit_data, static_cast<UINT>(prop_id), reinit_create_param);
	}
	void remove(std::int32_t prop_id) noexcept
	{
		mvRemoveProp(mV, serialized_edit_data, static_cast<UINT>(prop_id));
	}
	void remove(std::vector<PropData> const &props) noexcept
	{
		assert(!empty(props) && props.back().dwID == 0);
		mvRemoveProps(mV, serialized_edit_data, const_cast<PropData *>(data(props)));
	}

	template<typename T>
	static auto is_of_type(CPropValue *v) noexcept
	-> bool
	{
		if(auto temp = new (std::nothrow) T{})
		{
			bool const result = (temp->GetClassID() == v->GetClassID());
			temp->Delete();
			return result;
		}
		else
		{
			return false;
		}
	}
};
struct PropertiesSession final
{
	Data d;
	using PropId_t = std::int32_t;
	PropId_t next_prop_id = PROPID_EXTITEM_CUSTOM_FIRST;
	constexpr auto generate_prop_id()
	-> PropId_t
	{
		if(next_prop_id <= PROPID_EXTITEM_CUSTOM_LAST)
		{
			return next_prop_id++;
		}
		throw std::bad_alloc{};
	}
	PropId_t const id_version = generate_prop_id();
	PropId_t const id_num_lists = generate_prop_id();
	PropId_t const id_new_list = generate_prop_id();
	struct List final
	{
		Data::lists_t::iterator list;
		PropId_t id_folder, id_list_name, id_num_items, id_edit_mode, id_delim, id_esc, id_multipurpose;
		std::vector<PropId_t> id_items;
		bool num_items_checkbox {true};
		lSDK::string_t delim = TSL("\r\n");
		lSDK::string_t esc = TSL("\\");
		mutable std::vector<std::array<lSDK::char_t, std::numeric_limits<std::size_t>::digits10 + 1>> index_strs;
		enum struct EditMode
		: DWORD
		{
			Unselected = static_cast<DWORD>(-1),
			Multiline = 0,
			CheckboxInsert = 1,
			CheckboxDelete = 2,
		} edit_mode = EditMode::Unselected;

		List(PropertiesSession &session, decltype(list) list)
		: list{std::move(list)}
		, id_folder{session.generate_prop_id()}
		, id_list_name{session.generate_prop_id()}
		, id_num_items{session.generate_prop_id()}
		, id_edit_mode{session.generate_prop_id()}
		, id_delim{session.generate_prop_id()}
		, id_esc{session.generate_prop_id()}
		, id_multipurpose{session.generate_prop_id()}
		{
		}
		List(List const &) = delete;
		List(List &&) noexcept = default;
		List &operator=(List const &) = delete;
		List &operator=(List &&) noexcept = default;

		void generate(std::vector<PropData> &props) const
		{
			props.emplace_back(PropData{id_folder, reinterpret_cast<UINT_PTR>(list->first.c_str()), RILOPR_LIST_FOLDER_DESC, PROPTYPE_FOLDER, PROPOPT_NIL, 0});
			props.emplace_back(PropData{id_list_name, RILOPR_LIST_NAME_NAME, RILOPR_LIST_NAME_DESC, PROPTYPE_EDIT_MULTILINE, PROPOPT_CHECKBOX|PROPOPT_BOLD, 0});
			static constexpr std::array<std::int32_t, 2> const RANGE {0, 32767};
			props.emplace_back(PropData{id_num_items, RILOPR_LIST_SIZE_NAME, RILOPR_LIST_SIZE_DESC, PROPTYPE_SPINEDIT, PROPOPT_CHECKBOX|PROPOPT_PARAMREQUIRED, reinterpret_cast<LPARAM>(data(RANGE))});
			static constexpr std::array<std::uintptr_t, 5> const OPTIONS {0, RILOPR_EDIT_MODE_BUTTON, RILOPR_EDIT_MODE_INSERT, RILOPR_EDIT_MODE_DELETE, 0};
			props.emplace_back(PropData{id_edit_mode, RILOPR_EDIT_MODE_NAME, RILOPR_EDIT_MODE_DESC, PROPTYPE_COMBOBOX, PROPOPT_PARAMREQUIRED, reinterpret_cast<LPARAM>(data(OPTIONS))});
			if(edit_mode == EditMode::Multiline)
			{
				generate_multiline(props);
			}
			else if(edit_mode != EditMode::Unselected)
			{
				generate_list(props);
			}
			props.emplace_back(PropData{-1, 0, 0, PROPTYPE_FOLDER_END, PROPOPT_NIL, 0});
		}
		void generate_multiline(std::vector<PropData> &props) const
		{
			props.emplace_back(PropData{id_delim, RILOPR_DELIM_NAME, RILOPR_DELIM_DESC, PROPTYPE_EDIT_MULTILINE, PROPOPT_NIL, 0});
			props.emplace_back(PropData{id_esc, RILOPR_ESC_NAME, RILOPR_ESC_DESC, PROPTYPE_EDIT_MULTILINE, PROPOPT_NIL, 0});
			props.emplace_back(PropData{id_multipurpose, RILOPR_LIST_MULTILINE_NAME, RILOPR_LIST_MULTILINE_DESC, PROPTYPE_EDIT_MULTILINE, PROPOPT_NIL, 0});
		}
		void generate_item(std::vector<PropData> &props, std::size_t const i) const
		{
			assert(i < size(id_items) && i < size(index_strs));
			if(i < size(id_items) && i < size(index_strs))
			{
				std::swprintf(data(index_strs[i]), size(index_strs[i]), TSL("%zu"), i);
				props.emplace_back(PropData{id_items[i], reinterpret_cast<UINT_PTR>(data(index_strs[i])), RILOPR_LIST_ITEM_DESC, PROPTYPE_EDIT_MULTILINE, PROPOPT_CHECKBOX, 0});
			}
		}
		void generate_list(std::vector<PropData> &props) const
		{
			assert(size(id_items) >= size(list->second));
			props.emplace_back(PropData{id_multipurpose, RILOPR_NEW_ITEM_NAME, RILOPR_NEW_ITEM_DESC, PROPTYPE_EDITBUTTON, PROPOPT_NIL, RILOPR_NEW_ITEM_TEXT});
			index_strs.resize(size(list->second));
			for(std::size_t i = 0; i < size(list->second); ++i)
			{
				generate_item(props, i);
			}
		}

		void rename_self(PropertiesSession &session, PropsApi papi, lSDK::string_view_t const new_name)
		{
			if(session.d.lists.find(new_name) != end(session.d.lists))
			{
				return papi.refresh(id_list_name, false);
			}

			{ //perform the rename
				auto node = session.d.lists.extract(list);
				node.key().assign(new_name);
				list = session.d.lists.insert(std::move(node)).position;
				SerializedEditData::serialize(papi.mV, papi.serialized_edit_data, session.d);
			}

			{ //there's no way to rename a folder other than removing and replacing it
				//re-make the current folder
				std::vector<PropData> props;
				generate(props);
				props.emplace_back(PropData{});

				papi.remove(id_folder); //removing a folder also removes all the properties it contains and the end of the folder

				PropId_t insert_after = session.id_new_list;
				for(std::size_t i = 1; i < size(session.lists); ++i)
				{
					if(std::addressof(session.lists[i]) == this)
					{
						insert_after = session.lists[i - 1].id_folder; //insert after the previous folder (doesn't go inside the folder)
					}
				}
				return papi.insert(props, insert_after, true);
			}
		}
		void delete_self(PropertiesSession &session, PropsApi papi)
		{
			static bool already_asked = false;
			if(!already_asked && MessageBox(
				papi.mV->mvHEditWin,
				lSDK::string_t{lSDK::get_resource_string(RILOPR_CONFIRM_DELETE_MESSAGE)}.c_str(),
				lSDK::string_t{lSDK::get_resource_string(RILOPR_CONFIRM_DELETE_TITLE)}.c_str(),
				MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2|MB_TASKMODAL
			) == IDYES)
			{
				already_asked = true;
			}
			if(already_asked)
			{
				session.d.lists.erase(list);
				SerializedEditData::serialize(papi.mV, papi.serialized_edit_data, session.d);
				papi.remove(id_folder); //removing a folder also removes all the properties it contains and the end of the folder
				papi.refresh(session.id_num_lists, false);
				for(auto it = begin(session.lists); it != end(session.lists); )
				{
					if(it.operator->() == this)
					{
						it = session.lists.erase(it);
						return;
					}
				}
				assert(false);
			}
			return papi.refresh(id_list_name, false);
		}
		void refresh_items(PropertiesSession &session, PropsApi papi)
		{
			for(std::size_t i = 0; i < size(id_items) && i < size(list->second); ++i)
			{
				papi.refresh(id_items[i], false);
			}
		}
		void change_edit_mode(PropertiesSession &session, PropsApi papi, EditMode const new_edit_mode)
		{
			if(edit_mode != new_edit_mode)
			{
				EditMode const old_edit_mode = edit_mode;
				edit_mode = new_edit_mode;
				if(old_edit_mode == EditMode::Multiline)
				{
					std::vector<PropData> props;
					generate_multiline(props);
					props.emplace_back(PropData{});
					papi.remove(props);
				}
				if(new_edit_mode == EditMode::CheckboxInsert || new_edit_mode == EditMode::CheckboxDelete)
				{
					if(old_edit_mode == EditMode::CheckboxInsert || old_edit_mode == EditMode::CheckboxDelete)
					{
						refresh_items(session, papi);
					}
					else
					{
						id_items.reserve(size(list->second));
						while(size(id_items) < size(list->second))
						{
							id_items.emplace_back(session.generate_prop_id());
						}
						std::vector<PropData> props;
						generate_list(props);
						props.emplace_back(PropData{});
						papi.insert(props, id_edit_mode, true);
					}
				}
				else if(old_edit_mode == EditMode::CheckboxInsert || old_edit_mode == EditMode::CheckboxDelete)
				{
					std::vector<PropData> props;
					generate_list(props);
					props.emplace_back(PropData{});
					papi.remove(props);
				}
				if(new_edit_mode == EditMode::Multiline)
				{
					std::vector<PropData> props;
					generate_multiline(props);
					props.emplace_back(PropData{});
					papi.insert(props, id_edit_mode, true);
				}
			}
		}
		void insert_items(PropertiesSession &session, PropsApi papi, std::size_t const i, std::size_t const n)
		{
			assert(i <= size(list->second));
			if(i <= size(list->second) && n > 0)
			{
				PropId_t insert_after = id_multipurpose;
				if(!empty(id_items) && size(id_items) >= size(list->second))
				{
					insert_after = id_items[size(list->second) - 1];
				}
				std::size_t const old_size = size(list->second);
				list->second.insert(cbegin(list->second) + i, n, Data::list_t::value_type{});
				SerializedEditData::serialize(papi.mV, papi.serialized_edit_data, session.d);
				if(edit_mode == EditMode::Multiline)
				{
					papi.refresh(id_multipurpose, false);
				}
				else if(edit_mode == EditMode::CheckboxInsert || edit_mode == EditMode::CheckboxDelete)
				{
					index_strs.resize(size(list->second));
					id_items.reserve(size(list->second));
					while(size(id_items) < size(list->second))
					{
						id_items.emplace_back(session.generate_prop_id());
					}
					//since we can't edit the names of existing properties, we have to insert at the end and just refresh existing properties
					std::vector<PropData> props;
					for(std::size_t j = old_size; j < size(list->second); ++j)
					{
						generate_item(props, j);
					}
					props.emplace_back(PropData{});
					papi.insert(props, insert_after, true);
					for(std::size_t j = i; j < old_size; ++j)
					{
						papi.refresh(id_items[j], false);
					}
				}
			}
		}
		void erase_items(PropertiesSession &session, PropsApi papi, std::size_t const i, std::size_t n)
		{
			assert(i < size(list->second));
			if(i < size(list->second) && n > 0)
			{
				if(i + n > size(list->second))
				{
					n = size(list->second) - i;
				}
				std::size_t const old_size = size(list->second);
				list->second.erase(cbegin(list->second) + i, cbegin(list->second) + i + n);
				SerializedEditData::serialize(papi.mV, papi.serialized_edit_data, session.d);
				if(edit_mode == EditMode::Multiline)
				{
					papi.refresh(id_multipurpose, false);
				}
				else if(edit_mode == EditMode::CheckboxInsert || edit_mode == EditMode::CheckboxDelete)
				{
					//since we can't edit the names of existing properties, we have to erase at the end and just refresh existing properties
					std::vector<PropData> props;
					for(std::size_t j = old_size - n; j < old_size; ++j)
					{
						generate_item(props, j);
					}
					props.emplace_back(PropData{});
					papi.remove(props);
					for(std::size_t j = i; j < size(list->second); ++j)
					{
						papi.refresh(id_items[j], false);
					}
				}
			}
		}
		void on_item_checkbox(PropertiesSession &session, PropsApi papi, std::size_t const i)
		{
			if(edit_mode == EditMode::CheckboxInsert)
			{
				insert_items(session, papi, i, 1);
			}
			else if(edit_mode == EditMode::CheckboxDelete)
			{
				erase_items(session, papi, i, 1);
			}
			else
			{
				assert(false);
			}
			papi.refresh(id_num_items, false);
		}
		void on_push_new(PropertiesSession &session, PropsApi papi)
		{
			insert_items(session, papi, size(list->second), 1);
			papi.refresh(id_num_items, false);
		}
		void resize_num_items(PropertiesSession &session, PropsApi papi, std::size_t const new_size)
		{
			std::size_t const old_size = size(list->second);
			if(new_size < old_size)
			{
				if(num_items_checkbox)
				{
					erase_items(session, papi, new_size, old_size - new_size);
				}
				else
				{
					erase_items(session, papi, 0, old_size - new_size);
				}
			}
			else if(new_size > old_size)
			{
				if(num_items_checkbox)
				{
					insert_items(session, papi, old_size, new_size - old_size);
				}
				else
				{
					insert_items(session, papi, 0, new_size - old_size);
				}
			}
		}

		auto get_value(PropertiesSession const &session, PropId_t const prop_id) const
		-> CPropValue *
		{
			if(prop_id == id_list_name)     { return new CPropStringValue(list->first.c_str()); }
			else if(prop_id == id_num_items){ return new CPropDWordValue(static_cast<DWORD>(size(list->second))); }
			else if(prop_id == id_edit_mode){ return new CPropDWordValue(static_cast<DWORD>(edit_mode)); }
			else if(prop_id == id_delim)    { return new CPropStringValue(delim.c_str()); }
			else if(prop_id == id_esc)      { return new CPropStringValue(esc.c_str()); }
			else if(prop_id == id_multipurpose)
			{
				if(edit_mode == EditMode::Multiline)
				{
					return new CPropStringValue(combine_escaped(list->second, delim, esc).c_str());
				}
				return nullptr;
			}
			else if(edit_mode == EditMode::Unselected || edit_mode == EditMode::Multiline){}
			else if(auto const it = find(cbegin(id_items), cend(id_items), prop_id); it != cend(id_items))
			{
				std::size_t const i = (it - cbegin(id_items));
				assert(i < size(list->second));
				return new CPropStringValue(list->second.at(i).c_str());
			}
			return nullptr;
		}
		auto set_value(PropertiesSession &session, PropId_t const prop_id, PropsApi papi, CPropValue *v)
		-> bool
		{
			bool const is_string {PropsApi::is_of_type<CPropStringValue>(v) || PropsApi::is_of_type<CPropDataValue>(v)};
			if(is_string && prop_id == id_list_name)
			{
				rename_self(session, papi, static_cast<CPropStringValue *>(v)->GetString());
				return true;
			}
			else if(!is_string && prop_id == id_num_items)
			{
				if(PropsApi::is_of_type<CPropDWordValue>(v))
				{
					resize_num_items(session, papi, static_cast<CPropDWordValue *>(v)->m_dwValue);
				}
				else if(PropsApi::is_of_type<CPropIntValue>(v))
				{
					resize_num_items(session, papi, static_cast<std::size_t>(static_cast<CPropIntValue *>(v)->m_nValue));
				}
				else if(PropsApi::is_of_type<CPropInt64Value>(v))
				{
					resize_num_items(session, papi, static_cast<std::size_t>(static_cast<CPropInt64Value *>(v)->m_nValue));
				}
				return true;
			}
			else if(!is_string && prop_id == id_edit_mode)
			{
				if(PropsApi::is_of_type<CPropDWordValue>(v))
				{
					change_edit_mode(session, papi, static_cast<EditMode>(static_cast<CPropDWordValue *>(v)->m_dwValue));
				}
				return true;
			}
			else if(is_string && prop_id == id_delim)
			{
				delim = static_cast<CPropStringValue *>(v)->GetString();
				papi.refresh(id_multipurpose, false);
				return true;
			}
			else if(is_string && prop_id == id_esc)
			{
				esc = static_cast<CPropStringValue *>(v)->GetString();
				papi.refresh(id_multipurpose, false);
				return true;
			}
			else if(is_string && prop_id == id_multipurpose)
			{
				if(edit_mode == EditMode::Multiline)
				{
					{
						Data::list_t new_list;
						new_list.reserve(size(list->second));
						split_escaped(static_cast<CPropStringValue *>(v)->GetString(), delim, esc, [&](lSDK::string_view_t const item)
						{
							new_list.emplace_back(item);
						});
						list->second = std::move(new_list);
					}
					SerializedEditData::serialize(papi.mV, papi.serialized_edit_data, session.d);
					papi.refresh(id_num_items, false);
				}
				return true;
			}
			else if(!is_string || edit_mode == EditMode::Unselected || edit_mode == EditMode::Multiline){}
			else if(auto const it = find(cbegin(id_items), cend(id_items), prop_id); it != cend(id_items))
			{
				std::size_t const i = (it - cbegin(id_items));
				assert(i < size(list->second));
				list->second.at(i) = static_cast<CPropStringValue *>(v)->GetString();
				SerializedEditData::serialize(papi.mV, papi.serialized_edit_data, session.d);
				return true;
			}
			return false;
		}
	};
	std::vector<List> lists;

	auto generate() const
	-> std::vector<PropData>
	{
		std::vector<PropData> fusion_props;
		fusion_props.reserve(size(lists)*10);
		for(auto const &l : lists)
		{
			l.generate(fusion_props);
		}
		fusion_props.emplace_back(PropData{});
		return fusion_props;
	}

	PropertiesSession() = delete;
	PropertiesSession(PropertiesSession const &) = delete;
	PropertiesSession(PropertiesSession &&) = delete;
	PropertiesSession &operator=(PropertiesSession const &) = delete;
	PropertiesSession &operator=(PropertiesSession &&) = delete;
	PropertiesSession(PropsApi papi)
	: d{papi.serialized_edit_data->deserialize()}
	{
		std::vector<PropData> props
		{
			{id_version, RILOPR_VERSION_NAME, RILOPR_VERSION_DESC, PROPTYPE_STATIC, PROPOPT_BOLD, 0},
			{id_num_lists, RILOPR_NUM_LISTS_NAME, RILOPR_NUM_LISTS_DESC, PROPTYPE_STATIC, PROPOPT_NIL, 0},
			{id_new_list, RILOPR_NEW_LIST_NAME, RILOPR_NEW_LIST_DESC, PROPTYPE_EDITBUTTON, PROPOPT_NIL, RILOPR_NEW_LIST_TEXT}
		};
		for(auto it = begin(d.lists); it != end(d.lists); ++it)
		{
			lists.emplace_back(std::ref(*this), it).generate(props);
		}
		props.emplace_back(PropData{});
		papi.insert(props, PROPID_TAB_GENERAL, true);
	}

	constexpr auto is_enabled(PropId_t const prop_id) const noexcept
	-> bool
	{
		return prop_id != id_version;
	}
	auto is_ticked(PropId_t const prop_id) const noexcept
	-> bool
	{
		for(auto const &l : lists)
		{
			if(prop_id == l.id_list_name)
			{
				return true;
			}
			else if(prop_id == l.id_num_items)
			{
				return l.num_items_checkbox;
			}
			else if(l.edit_mode == List::EditMode::Unselected || l.edit_mode == List::EditMode::Multiline){}
			else if(find(cbegin(l.id_items), cend(l.id_items), prop_id) != cend(l.id_items))
			{
				return l.edit_mode == List::EditMode::CheckboxDelete;
			}
		}
		return false;
	}
	void set_ticked(PropId_t const prop_id, PropsApi papi, bool const ticked)
	{
		for(auto &l : lists)
		{
			if(prop_id == l.id_list_name)
			{
				return l.delete_self(*this, papi);
			}
			else if(prop_id == l.id_num_items)
			{
				l.num_items_checkbox = ticked;
				return;
			}
			else if(l.edit_mode == List::EditMode::Unselected || l.edit_mode == List::EditMode::Multiline){}
			else if(auto it = find(cbegin(l.id_items), cend(l.id_items), prop_id); it != cend(l.id_items))
			{
				return l.on_item_checkbox(*this, papi, (it - cbegin(l.id_items)));
			}
		}
	}
	auto get_value(PropId_t const prop_id) const
	-> CPropValue *
	{
		if(prop_id == id_version)
		{
			return new CPropStringValue(TSL("2.0.0"));
		}
		else if(prop_id == id_num_lists)
		{
			return new CPropStringValue(std::to_wstring(size(d.lists)).c_str());
		}
		else for(auto const &l : lists)
		{
			if(auto v = l.get_value(*this, prop_id))
			{
				return v;
			}
		}
		return nullptr;
	}
	void set_value(PropId_t const prop_id, PropsApi papi, CPropValue *v)
	{
		for(auto &l : lists)
		{
			if(l.set_value(*this, prop_id, papi, v))
			{
				return;
			}
		}
	}
	void on_press(PropId_t const prop_id, PropsApi papi)
	{
		if(prop_id == id_new_list)
		{
			lSDK::string_t name;
			for(std::size_t i = 0; ; ++i)
			{
				name = TSL("List ") + std::to_wstring(i);
				if(d.lists.find(name) == end(d.lists))
				{
					break;
				}
				if(i == std::numeric_limits<decltype(i)>::max())
				{
					return;
				}
			}
			auto it = d.lists.emplace(name, Data::list_t{}).first;
			SerializedEditData::serialize(papi.mV, papi.serialized_edit_data, d);
			std::vector<PropData> props;
			lists.emplace(begin(lists), std::ref(*this), it)->generate(props);
			props.emplace_back(PropData{});
			papi.insert(props, id_new_list, true);
			return papi.refresh(id_num_lists, false);
		}
		else for(auto &l : lists)
		{
			if(prop_id == l.id_multipurpose)
			{
				l.on_push_new(*this, papi);
			}
		}
	}
};
static std::uint64_t session_generator = 0;
static std::unordered_map<std::uint64_t, PropertiesSession> sessions;
#endif

#ifdef FUSION_GET_PROPERTIES
auto FUSION_API GetProperties(mv *const mV, SerializedEditData *serialized_edit_data, fusion::boolean const main_item) noexcept
-> fusion::boolean
{
	if(!main_item)
	{
		return FUSION_GET_PROPERTIES_FAILURE;
	}
	if(serialized_edit_data->eHeader.extVersion != SerializedEditData::CURRENT_VERSION)
	{
		//have to upgrade older properties in order to store the session without corrupting
		SerializedEditData::serialize(mV, serialized_edit_data, serialized_edit_data->deserialize());
	}
	serialized_edit_data->properties_session = ++session_generator; //we need the session because the SED address can change even when we don't touch it
	auto p = sessions.emplace(serialized_edit_data->properties_session, PropsApi{mV, serialized_edit_data});
	return FUSION_GET_PROPERTIES_SUCCESS;
}
#endif
#ifdef FUSION_GET_PROPERTY_PARAM
auto FUSION_API GetPropCreateParam(mv *const mV, SerializedEditData *serialized_edit_data, std::int32_t const property_id) noexcept
-> std::int32_t
{
	return {};
}
#endif
#ifdef FUSION_FREE_PROPERTY_PARAM
void FUSION_API ReleasePropCreateParam(mv *const mV, SerializedEditData *serialized_edit_data, std::int32_t const property_id, std::int32_t const param) noexcept
{
}
#endif
#ifdef FUSION_FREE_PROPERTIES
void FUSION_API ReleaseProperties(mv *const mV, SerializedEditData *serialized_edit_data, fusion::boolean const main_item) noexcept
{
	sessions.erase(serialized_edit_data->properties_session);
	serialized_edit_data->properties_session = 0;
}
#endif

#ifdef FUSION_IS_PROPERTY_ENABLED
auto FUSION_API IsPropEnabled(mv *const mV, SerializedEditData *serialized_edit_data, std::int32_t const property_id) noexcept
-> fusion::boolean
{
	if(auto const it = sessions.find(serialized_edit_data->properties_session); it != end(sessions))
	{
		return it->second.is_enabled(property_id)? FUSION_IS_PROPERTY_ENABLED_ENABLED : FUSION_IS_PROPERTY_ENABLED_DISABLED;
	}
	return FUSION_IS_PROPERTY_ENABLED_DISABLED;
}
#endif

#ifdef FUSION_GET_PROPERTY_VALUE
auto FUSION_API GetPropValue(mv *const mV, SerializedEditData *serialized_edit_data, std::int32_t const property_id) noexcept
-> CPropValue *
{
	if(auto const it = sessions.find(serialized_edit_data->properties_session); it != end(sessions))
	{
		return it->second.get_value(property_id);
	}
	return nullptr;
}
#endif
#ifdef FUSION_GET_PROPERTY_CHECKBOX
auto FUSION_API GetPropCheck(mv *const mV, SerializedEditData *serialized_edit_data, std::int32_t const property_id) noexcept
-> fusion::boolean
{
	if(auto const it = sessions.find(serialized_edit_data->properties_session); it != end(sessions))
	{
		return it->second.is_ticked(property_id)? FUSION_GET_PROPERTY_CHECKBOX_TICKED : FUSION_GET_PROPERTY_CHECKBOX_UNTICKED;
	}
	return FUSION_GET_PROPERTY_CHECKBOX_UNTICKED;
}
#endif
#ifdef FUSION_SET_PROPERTY_VALUE
void FUSION_API SetPropValue(mv *const mV, SerializedEditData *serialized_edit_data, std::int32_t const property_id, CPropValue *const property_value) noexcept
{
	if(auto const it = sessions.find(serialized_edit_data->properties_session); it != end(sessions))
	{
		it->second.set_value(property_id, PropsApi{mV, serialized_edit_data}, property_value);
	}
}
#endif
#ifdef FUSION_SET_PROPERTY_CHECKBOX
void FUSION_API SetPropCheck(mv *const mV, SerializedEditData *serialized_edit_data, std::int32_t const property_id, fusion::boolean const ticked) noexcept
{
	if(auto const it = sessions.find(serialized_edit_data->properties_session); it != end(sessions))
	{
		it->second.set_ticked(property_id, PropsApi{mV, serialized_edit_data}, ticked == FUSION_SET_PROPERTY_CHECKBOX_TICKED);
	}
}
#endif
#ifdef FUSION_PROPERTY_BUTTON_PRESSED
auto FUSION_API EditProp(mv *const mV, SerializedEditData *serialized_edit_data, std::int32_t const property_id) noexcept
-> fusion::boolean
{
	if(auto const it = sessions.find(serialized_edit_data->properties_session); it != end(sessions))
	{
		it->second.on_press(property_id, PropsApi{mV, serialized_edit_data});
		return FUSION_PROPERTY_BUTTON_PRESSED_SAVE_CHANGES;
	}
	return FUSION_PROPERTY_BUTTON_PRESSED_CANCEL_CHANGES;
}
#endif
