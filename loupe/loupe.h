// Copyright (c) 2022 Emilian Cioca
#pragma once
#include <any>
#include <memory>
#include <optional>
#include <string_view>
#include <variant>
#include <vector>

namespace loupe
{
	struct enum_descriptor;
	struct type_descriptor;
	struct member_descriptor;
	struct reflection_blob;

	// The starting point for retrieving reflection data. This cannot be
	// called before main() (during static initialization). You may still
	// store this result statically/globally in your program at the top
	// of your main() function, or whenever you wish afterwards.
	[[nodiscard]] reflection_blob reflect();

	// Clears the global static task lists, which will reclaim some memory.
	// However, calling reflect() afterwards will result in an empty result.
	void clear_reflect_tasks();

	namespace detail
	{
		struct type_handler_base
		{
			virtual ~type_handler_base() {}
		};

		template<typename Type>
		struct type_handler : public type_handler_base
		{
			static inline const type_descriptor* descriptor = nullptr;
		};

		std::vector<void(*)(reflection_blob&)>& get_enum_descriptor_tasks();
		std::vector<void(*)(reflection_blob&)>& get_type_descriptor_tasks();
		std::vector<void(*)(reflection_blob&)>& get_member_descriptor_tasks();

		template<typename T> [[nodiscard]]
		consteval std::string_view get_type_name();
	}

	struct member_descriptor
	{
		std::size_t offset;
		std::string_view name;
		const type_descriptor* type;
		const type_descriptor* owning_type;

		std::vector<const type_descriptor*> metadata;

		template<typename Tag> [[nodiscard]]
		bool has_metadata() const;
	};

	struct enum_entry
	{
		std::string_view name;
		std::size_t value;
	};

	struct enum_descriptor
	{
		std::string_view name;
		const type_descriptor* underlying_type;
		std::vector<enum_entry> entires;

		template<typename T> [[nodiscard]]
		T get_value_from_name(std::string_view name) const;

		template<typename T> [[nodiscard]]
		std::string_view get_name_from_value(T value) const;
	};


	struct type_descriptor
	{
		template<typename Type>
		type_descriptor(const detail::type_handler<Type>& type_handler);

		[[nodiscard]] std::any make_new();

		[[nodiscard]] const member_descriptor* find_member(std::string_view name) const;
		[[nodiscard]] const member_descriptor* find_member(std::size_t offset) const;
		[[nodiscard]] const member_descriptor* find_first_of(const type_descriptor& type) const;

		[[nodiscard]] bool is_a(const type_descriptor* type) const;

		std::vector<const member_descriptor*> members;

		std::string_view name;
		const type_descriptor* parent = nullptr;

		const detail::type_handler_base* type_handler;
	};

	struct reflection_blob
	{
		template<typename Type>
		[[nodiscard]] auto find() const -> std::conditional_t<std::is_enum_v<Type>, const enum_descriptor*, const type_descriptor*>;
		[[nodiscard]] const type_descriptor* find_type(std::string_view name) const;
		[[nodiscard]] const enum_descriptor* find_enum(std::string_view name) const;

		// serialize functions

		std::vector<type_descriptor> types;
		std::vector<enum_descriptor> enums;
		std::vector<member_descriptor> members;

		std::vector<std::unique_ptr<detail::type_handler_base>> type_handlers;
	};
}


#define LOUPE_CONCATENATE(s1, s2) s1##s2
#define LOUPE_CONCATENATE_INDIRECT(s1, s2) LOUPE_CONCATENATE(s1, s2)
#define LOUPE_ANONYMOUS_VARIABLE(str) LOUPE_CONCATENATE_INDIRECT(str, __COUNTER__)

#define REFLECT_ENUM(enum_name) \
	const auto& LOUPE_ANONYMOUS_VARIABLE(dummy_) = loupe::detail::get_enum_descriptor_tasks().emplace_back([](loupe::reflection_blob& blob) \
	{                                                                                                                                       \
		using enum enum_name;                                                                                                               \
		using underlying_type = std::underlying_type_t<enum_name>;                                                                          \
		static_assert(sizeof(underlying_type) <= sizeof(std::size_t));                                                                      \
		static_assert(sizeof(std::is_convertible_v<underlying_type, std::size_t>));                                                         \
		static_assert(sizeof(std::is_convertible_v<std::size_t, underlying_type>));                                                         \
		loupe::enum_descriptor descriptor;                                                                                                  \
		descriptor.name = #enum_name;                                                                                                       \
		descriptor.underlying_type = blob.find<underlying_type>();                                                                          \
		descriptor.entires =
#define REF_VALUE(value) { #value, static_cast<std::size_t>(value) }
#define REF_ENUM_END ; blob.enums.push_back(std::move(descriptor)); });

#include "loupe.inl"
