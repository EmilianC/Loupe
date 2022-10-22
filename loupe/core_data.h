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

	//
	struct enum_entry
	{
		std::string_view name;
		std::size_t value;

		std::vector<const type*> metadata;

		template<typename Tag> [[nodiscard]]
		bool has_metadata() const;
	};

	//
	struct variable
	{
		std::string_view name;
		std::size_t offset;
		const type* type = nullptr;

		// Can the value be changed.
		bool is_const = false;

		std::vector<const struct type*> metadata;

		template<typename Tag> [[nodiscard]]
		bool has_metadata() const;
	};

	//
	struct static_variable
	{
		std::string_view name;
		void* address = nullptr;
		const type* type = nullptr;

		// Can the value be changed.
		bool is_const = false;

		std::vector<const struct type*> metadata;

		template<typename Tag> [[nodiscard]]
		bool has_metadata() const;
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
		bool is_target_const;

		void* dereference();

		// detail.
		void* (*dereference_implementation)(void*);
	};

	//
	struct array_type
	{
		const type* element_type = nullptr;
		bool dynamic;

		std::size_t (*get_count_implementation)(const void* array) = nullptr;
		bool  (*empty_implementation)(const void* array) = nullptr;
		bool  (*contains_implementation)(const void* array, const void* data) = nullptr;
		void* (*get_at_implementation)(void* array, std::size_t index) = nullptr;
		void  (*set_at_implementation)(void* array, std::size_t index, const void* data) = nullptr;
		void* (*emplace_back_implementation)(void* array, const void* data) = nullptr;
	};

	//
	struct map_type
	{
		const type* key_type = nullptr;
		const type* value_type = nullptr;

		std::size_t (*get_count_implementation)(const void* array);
		bool  (*contains_implementation)(const void* array);
		void* (*find_implementation)(void* array, const void* key);
		void  (*insert_implementation)(void* array, const void* key, const void* value);
	};

	//
	struct variant_type
	{
		std::vector<const type*> alternatives;

		std::size_t (*get_index_implementation)(const void* variant);
	};

	//
	struct fundamental_type {};

	//
	struct type
	{
		std::string_view name;
		std::size_t size;
		std::size_t alignment;
		bool default_constructible;

		std::variant<class_type, enum_type, pointer_type, array_type, map_type, variant_type, fundamental_type> data;

		[[nodiscard]] bool is_a(const type&) const;

		[[nodiscard]] std::any construct() const;
		void construct_at(void* location) const;

		template<typename... Visitors>
		auto visit(Visitors&&... visitors) const;

		std::any (*construct_implementation)() = nullptr;
		void (*construct_at_implementation)(void*) = nullptr;
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
