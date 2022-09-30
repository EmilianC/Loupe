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

		enum class stage
		{
			types, bases_and_members
		};

		std::vector<void(*)(reflection_blob&)>&                          get_enum_descriptor_tasks();
		std::vector<void(*)(reflection_blob&, type_descriptor&, stage)>& get_type_descriptor_tasks();

		template<typename T> [[nodiscard]]
		consteval std::string_view get_type_name();
	}

	struct member_descriptor
	{
		std::size_t offset;
		std::string_view name;
		const type_descriptor* type = nullptr;
		const enum_descriptor* enum_type = nullptr;
		const type_descriptor* owning_type;

		// Can the member value be changed.
		bool is_const = false;
		// Is the value an address to the type.
		bool is_pointer = false;
		// Is the referenced address immutable.
		bool is_const_pointer = false;

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

		[[nodiscard]] const std::size_t*      find_value_from_name(std::string_view entry_name) const;
		[[nodiscard]] const std::string_view* find_name_from_value(std::size_t value) const;
	};


	struct type_descriptor
	{
		template<typename Type>
		type_descriptor(const detail::type_handler<Type>& type_handler);
		type_descriptor() = default;

		[[nodiscard]] std::any make_new();

		[[nodiscard]] const member_descriptor* find_member(std::string_view member_name) const;
		[[nodiscard]] const member_descriptor* find_member(std::size_t offset) const;
		[[nodiscard]] const member_descriptor* find_first_of(const type_descriptor& type) const;
		[[nodiscard]] const member_descriptor* find_first_of(const enum_descriptor& type) const;

		[[nodiscard]] bool is_a(const type_descriptor* type) const;

		std::string_view name;
		std::vector<member_descriptor> members;
		std::vector<const type_descriptor*> bases;

		const detail::type_handler_base* type_handler;
	};

	struct reflection_blob
	{
		template<typename Type>
		[[nodiscard]] auto                   find() const -> std::conditional_t<std::is_enum_v<Type>, const enum_descriptor*, const type_descriptor*>;
		[[nodiscard]] const type_descriptor* find_type(std::string_view name) const;
		[[nodiscard]] const enum_descriptor* find_enum(std::string_view name) const;

		// serialize functions

		std::vector<type_descriptor> types;
		std::vector<enum_descriptor> enums;

		std::vector<std::unique_ptr<detail::type_handler_base>> type_handlers;
	};
}


#define LOUPE_CONCATENATE(s1, s2) s1##s2
#define LOUPE_CONCATENATE_INDIRECT(s1, s2) LOUPE_CONCATENATE(s1, s2)
#define LOUPE_ANONYMOUS_VARIABLE(str) LOUPE_CONCATENATE_INDIRECT(str, __COUNTER__)


/// Enums ///
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

#define FRIEND_LOUPE

/// Structures and Classes ///
#define REFLECT(type_name)
#define REF_BASES(...) register_bases<__VA_ARGS__()>(blob, *type);
#define REF_BASE(...) REF_BASES(__VA_ARGS__())
#define REF_MEMBER(member, ...) register_member<decltype(class_type::member), __VA_ARGS__()>(blob, type, "member", offsetof(class_type, member)),
#define REF_END }; } break; } });

#include "loupe.inl"
