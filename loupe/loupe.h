// Copyright (c) 2022 Emilian Cioca
#pragma once
#include <string_view>
#include <vector>
#include <memory>
#include <any>

namespace loupe
{
	struct member_descriptor;
	struct type_descriptor;
	struct reflection_blob;

	namespace detail
	{
		struct type_handler_base
		{
			virtual ~type_handler_base() {}
		};

		template<typename Type>
		struct type_handler : public type_handler_base
		{
			static inline type_descriptor* descriptor = nullptr;
		};

		std::vector<void(*)(reflection_blob&)>& get_type_descriptor_tasks();
		std::vector<void(*)(reflection_blob&)>& get_member_descriptor_tasks();
	}

	struct member_descriptor
	{
		std::size_t offset;
		std::string_view name;
		type_descriptor* type_name;
		type_descriptor* owning_type;
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

		std::vector<member_descriptor> members;

		std::string_view name;
		std::string_view full_name;
		const type_descriptor* parent = nullptr;

		const detail::type_handler_base* type_handler;
	};

	struct reflection_blob
	{
		template<typename Type>
		[[nodiscard]] const type_descriptor* find_type() const;
		[[nodiscard]] const type_descriptor* find_type(std::string_view name) const;

		// serialize functions

		std::vector<type_descriptor> types;
		std::vector<member_descriptor> members;

		std::vector<std::unique_ptr<detail::type_handler_base>> type_handlers;
	};

	[[nodiscard]] reflection_blob reflect();
	void clear_reflect_tasks();
}

#include "loupe.inl"
