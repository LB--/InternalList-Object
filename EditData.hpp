#pragma once

#include "FusionAPI.hpp"
#include "Data.hpp"

struct SerializedEditData final
{
	static constexpr std::int16_t CURRENT_VERSION = 4;
	extHeader eHeader;

	std::uint64_t properties_session;

	std::byte raw[sizeof(std::uint32_t)];

	static void serialize(mv *const, SerializedEditData *&, Data const &);
	auto deserialize() const -> Data;

	SerializedEditData() = delete;
	SerializedEditData(SerializedEditData const &) = delete;
	SerializedEditData(SerializedEditData &&) = delete;
	SerializedEditData &operator=(SerializedEditData &&) = delete;
	SerializedEditData &operator=(SerializedEditData const &) = delete;
	~SerializedEditData() = delete;
};

static_assert(fusion::is_valid_editdata_structure());
