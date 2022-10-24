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
	struct variable : public metadata_container
	{
		std::string_view name;
		std::size_t offset;
		const type* type = nullptr;
		bool is_const = false;
	};

	//
	struct static_variable : public metadata_container
	{
		std::string_view name;
		void* address = nullptr;
		const type* type = nullptr;
		bool is_const = false;
	};

	//
	struct class_type
	{
		std::vector<variable> variables;
		std::vector<static_variable> static_variables;
		std::vector<const type*> bases;

		[[nodiscard]] bool is_derived_from(const type&) const;

		[[nodiscard]] const variable* find_variable(std::string_view var_name) const;
		[[nodiscard]] const variable* find_variable(std::size_t offset) const;
	};

	//
	struct enum_type
	{
		const type* underlying_type = nullptr;
		std::vector<enum_entry> entries;

		[[nodiscard]] const std::size_t*      find_enum_value(std::string_view value_name) const;
		[[nodiscard]] const std::string_view* find_enum_name(std::size_t value) const;
	};

	//
	struct pointer_type
	{
		const type* target_type = nullptr;
		bool is_target_const = false;

		void* (*dereference)(void* pointer) = nullptr;
	};

	//
	struct array_type
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
	struct map_type
	{
		const type* key_type = nullptr;
		const type* value_type = nullptr;

		std::size_t (*get_count)(const void* map) = nullptr;
		bool  (*contains)(const void* map) = nullptr;
		void* (*find)(void* map, const void* key) = nullptr;
		void  (*insert)(void* map, const void* key, const void* value) = nullptr;
	};

	//
	struct variant_type
	{
		std::vector<const type*> alternatives;

		bool (*holds_value)(const void* variant) = nullptr;
		std::size_t (*get_index)(const void* variant) = nullptr;
		void* (*get_value)(void* variant) = nullptr;
	};

	//
	struct fundamental_type {};

	//
	struct type
	{
		std::string_view name;
		std::size_t size;
		std::size_t alignment;

		std::variant<class_type, enum_type, pointer_type, array_type, map_type, variant_type, fundamental_type> data;

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
