// Copyright (c) 2022 Emilian Cioca
#pragma once
#include "private_members.h"

#include <any>
#include <memory>
#include <string_view>
#include <variant>
#include <vector>

namespace loupe
{
	struct type;
	struct property;

	//
	struct pointer
	{
		const property* target_property = nullptr;
		bool is_target_const = false;

		void* (*get_target_address)(void* pointer) = nullptr;
		std::shared_ptr<void> (*get_shared)(void* pointer) = nullptr;
		void (*reset)(void* pointer, void* new_target) = nullptr;
	};

	//
	struct array
	{
		const property* element_property = nullptr;
		bool dynamic = false;
		// True if the container only permits unique elements.
		bool unique_set = false;

		std::size_t (*get_count)(const void* array) = nullptr;
		bool (*contains)(const void* array, const void* data) = nullptr;
		const void* (*get_at)(const void* array, std::size_t index) = nullptr;
		void (*set_at)(void* array, std::size_t index, const void* data) = nullptr;
		void (*emplace_back)(void* array, const void* data) = nullptr;
	};

	//
	struct map
	{
		const property* key_property = nullptr;
		const property* value_property = nullptr;

		std::size_t (*get_count)(const void* map) = nullptr;
		bool (*contains)(const void* map, const void* key) = nullptr;
		void* (*find)(void* map, const void* key) = nullptr;
		void  (*insert)(void* map, const void* key, const void* value) = nullptr;
	};

	//
	struct variant
	{
		std::vector<const property*> alternatives;

		std::size_t (*get_index)(const void* variant) = nullptr;
		void* (*get_value)(void* variant) = nullptr;
	};

	//
	struct property
	{
		std::string_view signature;
		std::variant<const type*, pointer, array, map, variant> data;

		template<typename... Visitors>
		auto visit(Visitors&&... visitors) const;

		[[nodiscard]] bool operator==(const property& other) const;
		[[nodiscard]] bool operator!=(const property& other) const;
	};

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
		const property* data = nullptr;
	};

	//
	struct fundamental {};

	//
	struct structure
	{
		std::vector<member> members;
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
	struct type
	{
		std::string_view name;
		std::size_t size;
		std::size_t alignment;

		std::variant<fundamental, structure, enumeration> data;

		[[nodiscard]] bool is_a(const type&) const;

		template<typename... Visitors>
		auto visit(Visitors&&... visitors) const;

		std::any (*construct)() = nullptr;
		void (*construct_at)(void* location) = nullptr;
	};

	//
	struct reflection_blob
	{
		reflection_blob() = default;
		reflection_blob(reflection_blob&) = delete;
		reflection_blob(reflection_blob&&) = default;
		reflection_blob& operator=(reflection_blob&) = delete;
		reflection_blob& operator=(reflection_blob&&) = default;

		[[nodiscard]] unsigned int get_version() const;
		[[nodiscard]] const std::vector<type>& get_types() const;
		[[nodiscard]] const std::vector<property>& get_properties() const;

		template<typename Type>
		[[nodiscard]] const type* find() const;
		[[nodiscard]] const type* find(std::string_view name) const;

		template<typename Type>
		[[nodiscard]] const property* find_property() const;
		[[nodiscard]] const property* find_property(std::string_view signature) const;
		
	private:
		friend reflection_blob reflect(unsigned int);

		unsigned int version = 0;
		std::vector<type> types;
		std::vector<property> properties;

	public:
		PRIVATE_MEMBER(reflection_blob, version);
		PRIVATE_MEMBER(reflection_blob, types);
		PRIVATE_MEMBER(reflection_blob, properties);
	};
}

#include "core_data.inl"
