// Copyright (c) 2022 Emilian Cioca
#pragma once
#include <any>
#include <memory>
#include <string_view>
#include <variant>
#include <vector>

namespace loupe
{
	struct type;

	struct metadata_container
	{
		template<typename Tag> [[nodiscard]]
		bool has_metadata() const;

		std::vector<const type*> metadata;
	};

	//
	struct enum_entry : public metadata_container
	{
		std::string_view name;
		std::size_t value;
	};

	//
	struct member : public metadata_container
	{
		std::string_view name;
		std::size_t offset;
		const type* type = nullptr;
		bool is_const = false;
	};

	//
	struct static_member : public metadata_container
	{
		std::string_view name;
		void* address = nullptr;
		const type* type = nullptr;
		bool is_const = false;
	};

	//
	struct structure
	{
		std::vector<member> members;
		std::vector<static_member> static_members;
		std::vector<const type*> bases;

		[[nodiscard]] bool is_derived_from(const type&) const;

		[[nodiscard]] const member* find_member(std::string_view var_name) const;
		[[nodiscard]] const member* find_member(std::size_t offset) const;
	};

	//
	struct enumeration
	{
		const type* underlying_type = nullptr;
		std::vector<enum_entry> entries;
		bool strongly_typed = false;

		[[nodiscard]] const std::size_t*      find_value(std::string_view value_name) const;
		[[nodiscard]] const std::string_view* find_name(std::size_t value) const;
	};

	//
	struct pointer
	{
		const type* target_type = nullptr;

		void* (*get_target_address)(void* pointer) = nullptr;
		std::shared_ptr<void> (*get_shared)(void* pointer) = nullptr;
		void (*reset)(void* pointer, void* new_target) = nullptr;
	};

	//
	struct array
	{
		const type* element_type = nullptr;
		bool dynamic = false;

		std::size_t (*get_count)(const void* array) = nullptr;
		bool (*empty)(const void* array) = nullptr;
		bool (*contains)(const void* array, const void* data) = nullptr;
		const void* (*get_at)(const void* array, std::size_t index) = nullptr;
		void (*set_at)(void* array, std::size_t index, const void* data) = nullptr;
		void (*emplace_back)(void* array, const void* data) = nullptr;
	};

	//
	struct map
	{
		const type* key_type = nullptr;
		const type* value_type = nullptr;

		std::size_t (*get_count)(const void* map) = nullptr;
		bool  (*contains)(const void* map) = nullptr;
		void* (*find)(void* map, const void* key) = nullptr;
		void  (*insert)(void* map, const void* key, const void* value) = nullptr;
	};

	//
	struct variant
	{
		std::vector<const type*> alternatives;

		bool (*holds_value)(const void* variant) = nullptr;
		std::size_t (*get_index)(const void* variant) = nullptr;
		void* (*get_value)(void* variant) = nullptr;
	};

	//
	struct fundamental {};

	//
	struct type
	{
		std::string_view name;
		std::size_t size;
		std::size_t alignment;

		std::variant<structure, enumeration, pointer, array, map, variant, fundamental> data;

		[[nodiscard]] bool is_a(const type&) const;

		template<typename... Visitors>
		auto visit(Visitors&&... visitors) const;

		std::any (*construct)() = nullptr;
		void (*construct_at)(void* location) = nullptr;
	};

	//
	struct reflection_blob
	{
		template<typename Type>
		[[nodiscard]] const type* find() const;
		[[nodiscard]] const type* find(std::string_view name) const;

		std::vector<type> types;
	};
}

#include "core_data.inl"
