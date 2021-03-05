#include "FusionAPI.hpp"
#include "EditData.hpp"
#include "RunData.hpp"

#ifdef FUSION_GET_RUNTIME_STRUCTURE_SIZE
auto FUSION_API GetRunObjectDataSize(RunHeader *const run_header, SerializedEditData const *const serialized_edit_data) noexcept
-> std::uint16_t
{
	return sizeof(RunData);
}
#endif

#ifdef FUSION_FINISH_RUNTIME_STRUCTURE_CONSTRUCTION
auto FUSION_API CreateRunObject(RunData *const run_data, SerializedEditData const *const serialized_edit_data, createObjectInfo *const create_object_info) noexcept
-> std::int16_t
{
	try
	{
		run_data->local_data = new Data{serialized_edit_data->deserialize()};
		run_data->current_list_name = new Data::lists_t::key_type;
		run_data->global_data_name = nullptr;
		return FUSION_FINISH_RUNTIME_STRUCTURE_CONSTRUCTION_SUCCESS;
	}
	catch(...)
	{
	}
	return FUSION_FINISH_RUNTIME_STRUCTURE_CONSTRUCTION_FAILURE;
}
#endif
#ifdef FUSION_BEGIN_RUNTIME_STRUCTURE_DESTRUCTION
auto FUSION_API DestroyRunObject(RunData *const run_data, std::int32_t const fast) noexcept
-> std::int16_t
{
	delete run_data->global_data_name;
	delete run_data->current_list_name;
	delete run_data->local_data;
	return FUSION_BEGIN_RUNTIME_STRUCTURE_DESTRUCTION_SUCCESS;
}
#endif

#ifdef FUSION_ON_TICK
auto FUSION_API HandleRunObject(RunData *const run_data) noexcept
-> std::int16_t
{
	return FUSION_ON_TICK_STOP_CALLING;
}
#endif

#ifdef FUSION_SERIALIZE_RUNTIME
auto FUSION_API SaveRunObject(RunData *const run_data, HANDLE const file_handle) noexcept
-> fusion::boolean
{
	//TODO
	return FUSION_SERIALIZE_RUNTIME_SUCCESS;
}
#endif
#ifdef FUSION_DESERIALIZE_RUNTIME
auto FUSION_API LoadRunObject(RunData *const run_data, HANDLE const file_handle) noexcept
-> fusion::boolean
{
	//TODO
	return FUSION_DESERIALIZE_RUNTIME_SUCCESS;
}
#endif

#ifdef FUSION_PAUSE_RUNTIME
auto FUSION_API PauseRunObject(RunData *const run_data) noexcept
-> std::int16_t
{
	return FUSION_PAUSE_RUNTIME_SUCCESS;
}
#endif
#ifdef FUSION_UNPAUSE_RUNTIME
auto FUSION_API ContinueRunObject(RunData *const run_data) noexcept
-> std::int16_t
{
	return FUSION_UNPAUSE_RUNTIME_SUCCESS;
}
#endif

Data::global_t Data::global;

#ifdef FUSION_START_APPLICATION
void FUSION_API StartApp(mv *const mV, CRunApp *const app) noexcept
{
}
#endif
#ifdef FUSION_END_APPLICATION
void FUSION_API EndApp(mv *const mV, CRunApp *const app) noexcept
{
	if(app->m_pParentApp == nullptr)
	{
		Data::global.clear();
	}
}
#endif
