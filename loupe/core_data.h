// Copyright (c) 2022 Emilian Cioca
#pragma once
#include "assert.h"
#include "private_members.h"

#include <any>
#include <concepts>
#include <memory>
#include <span>
#include <string_view>
#include <type_traits>
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

		// Visitor pattern convenience function for the possible variants of data.
		template<typename... Visitors>
		auto visit(Visitors&&... visitors) const;

		// Returns the property's data casted to the correct variant type. Asserts if not possible.
		// "fundamental", "structure", and "enumeration" are also valid template options.
		template<typename Data> [[nodiscard]]
		const Data& get_as() const;

		// Returns the property's data casted to the correct variant type, if possible.
		// "fundamental", "structure", and "enumeration" are also valid template options.
		template<typename Data> [[nodiscard]]
		const Data* try_as() const;

		[[nodiscard]] bool operator==(const property& other) const;
		[[nodiscard]] bool operator!=(const property& other) const;
	};

	struct metadata_container
	{
		template<typename Tag> [[nodiscard]]
		bool has() const;

		template<typename Tag> [[nodiscard]]
		const Tag* find() const;

		struct entry { const type* type; std::any value; };
		std::vector<entry> entries;
	};

	//
	struct member
	{
		std::string_view name;
		std::size_t offset;
		const property* data = nullptr;
		metadata_container metadata;

		// Helper function to retrieve the pointer to a member from the owning object.
		// It is the user's responsibility to ensure the requested type matches the property's signature.
		template<typename To = void> [[nodiscard]]
		To* offset_from(void* base_struct_pointer) const;

		template<typename To = void> [[nodiscard]]
		const To* offset_from(const void* base_struct_pointer) const;

		// Returns a direct copy of the member value, or the result of calling the reflected getter.
		template<typename To> [[nodiscard]]
		To get_copy_from(const void* base_struct_pointer) const;

		// Sets the member directly or calls the setter function if one was reflected.
		template<typename From>
		void set_on(void* base_struct_pointer, From value) const;

		void* getter = nullptr;
		void* setter = nullptr;
	};

	//
	struct base
	{
		const type* type = nullptr;
		std::size_t offset;
	};

	//
	struct fundamental {};

	//
	struct structure
	{
		std::vector<member> members;
		std::vector<base> bases;

		// Iterates over all member reflection data, including those from inherited bases.
		// Members which are structures are NOT recursed into, however this can be
		// accomplished by calling walk_members() again within the provided functor.
		template<typename Functor> requires std::invocable<Functor, const member&>
		void walk_members(Functor&&) const;

		// Iterates over the members of an object instance, including any from inherited bases.
		// The base_struct_pointer is offset for you to match the start of any bases.
		// Members which are structures must be recursed into manually during the functor.
		template<typename Functor> requires std::invocable<Functor, const member&, void*>
		void walk_members(void* base_struct_pointer, Functor&&) const;

		template<typename Functor> requires std::invocable<Functor, const member&, const void*>
		void walk_members(const void* base_struct_pointer, Functor&&) const;

		[[nodiscard]] bool is_derived_from(const type&) const;

		[[nodiscard]] const member* find_member(std::string_view name) const;
		[[nodiscard]] const member* find_member(std::size_t offset) const;
	};

	//
	struct enum_entry
	{
		std::string_view name;
		std::uint16_t value;
		metadata_container metadata;
	};

	//
	struct enumeration
	{
		std::vector<enum_entry> entries;
		bool strongly_typed = false;

		[[nodiscard]] const std::uint16_t*    find_value(std::string_view name) const;
		[[nodiscard]] const std::string_view* find_name(std::uint16_t value) const;

		[[nodiscard]] const enum_entry* find_entry(std::string_view name) const;
		[[nodiscard]] const enum_entry* find_entry(std::uint16_t value) const;
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

		// Calls the user constructor reflected through the USER_CONSTRUCTOR(...) macro.
		// ! Must be called with exactly the reflected arguments specified in the macro !
		template<typename... Args>
		void user_construct_at(void* location, Args&&... args) const;

		// Only public constructors are bound.
		void (*default_construct_at)(void* location) = nullptr;
		// Only public destructors that are non-trivial are bound.
		void (*destruct_at)(void* location) = nullptr;

		void* user_constructor = nullptr;
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
		[[nodiscard]] std::span<const type> get_types() const;
		[[nodiscard]] std::span<const property> get_properties() const;

		template<typename Type>
		[[nodiscard]] const type* find() const;
		[[nodiscard]] const type* find(std::string_view name) const;

		template<typename Property>
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

	// Finds the lowest common base between two types.
	// This function is most meaningful for structure types, otherwise it is an equality check.
	// Both provided structure types must have strictly linear hierarchies.
	[[nodiscard]] const type* find_common_ancestor(const type&, const type&);
}

#include "core_data.inl"
