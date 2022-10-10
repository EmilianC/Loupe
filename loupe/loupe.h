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

		template<typename Type> [[nodiscard]]
		consteval std::string_view get_type_name();

		template<typename Type, typename... Tags> [[nodiscard]]
		loupe::member_descriptor register_member(loupe::reflection_blob& blob, const loupe::type_descriptor& owning_type, std::string_view name, std::size_t offset);

		template<typename Base> [[nodiscard]]
		const loupe::type_descriptor* register_base(loupe::reflection_blob& blob);
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
		std::size_t size;
		std::size_t alignment;
		bool default_constructible;
		std::vector<member_descriptor> members;
		std::vector<const type_descriptor*> bases;

		const detail::type_handler_base* type_handler;
	};

	struct reflection_blob
	{
		template<typename Type> [[nodiscard]] auto find() const -> std::conditional_t<std::is_enum_v<Type>, const enum_descriptor*, const type_descriptor*>;
	  //template<typename Type> [[nodiscard]] auto find_type() const -> const type_descriptor*; // need this?
	  //template<typename Type> [[nodiscard]] auto find_enum() const -> const enum_descriptor*; // need this?
		                        [[nodiscard]] auto find_type(std::string_view name) const -> const type_descriptor*;
								[[nodiscard]] auto find_enum(std::string_view name) const -> const enum_descriptor*;

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
#define REFLECT_ENUM(enum_name)                                                                                    \
	static const auto& LOUPE_ANONYMOUS_VARIABLE(dummy_) = loupe::detail::get_enum_descriptor_tasks().emplace_back( \
	[](loupe::reflection_blob& blob)                                                                               \
	{                                                                                                              \
		using enum enum_name;                                                                                      \
		using underlying_type = std::underlying_type_t<enum_name>;                                                 \
		static_assert(sizeof(underlying_type) <= sizeof(std::size_t));                                             \
		static_assert(sizeof(std::is_convertible_v<underlying_type, std::size_t>));                                \
		static_assert(sizeof(std::is_convertible_v<std::size_t, underlying_type>));                                \
		loupe::enum_descriptor descriptor;                                                                         \
		descriptor.name = #enum_name;                                                                              \
		descriptor.underlying_type = blob.find<underlying_type>();                                                 \
		descriptor.entires =
#define REF_VALUE(value) { #value, static_cast<std::size_t>(value) }
#define REF_ENUM_END ; blob.enums.push_back(std::move(descriptor)); });

#define FRIEND_LOUPE

/// Structures and Classes ///
#define REFLECT(type_name)                                                                                         \
	static const auto& LOUPE_ANONYMOUS_VARIABLE(dummy_) = loupe::detail::get_type_descriptor_tasks().emplace_back( \
	[](loupe::reflection_blob& blob, loupe::type_descriptor& type, loupe::detail::stage stage)                     \
	{                                                                                                              \
		using class_type = type_name;                                                                              \
		switch (stage)                                                                                             \
		{                                                                                                          \
		case loupe::detail::stage::types:                                                                          \
			type.name = #type_name;                                                                                \
			if constexpr (std::is_void_v<class_type>)                                                              \
			{                                                                                                      \
				type.size = 0;                                                                                     \
				type.alignment = 1;                                                                                \
				type.default_constructible = false;                                                                \
			}                                                                                                      \
			else                                                                                                   \
			{                                                                                                      \
				type.size = sizeof(class_type);                                                                    \
				type.alignment = alignof(class_type);                                                              \
				type.default_constructible = std::is_default_constructible_v<class_type>;                          \
			}                                                                                                      \
		break;                                                                                                     \
		case loupe::detail::stage::bases_and_members:
#define BASES ; type.bases =
#define REF_BASE(base_type) loupe::detail::register_base<base_type>(blob),
#define MEMBERS ; type.members =
#define REF_MEMBER(member, ...) loupe::detail::register_member<decltype(class_type::member), __VA_ARGS__>(blob, type, #member, offsetof(class_type, member))
#define REF_END ;}});

#include "loupe.inl"
