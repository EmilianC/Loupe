// Copyright (c) 2022 Emilian Cioca
#pragma once
#include "assert.h"
#include "private_members.h"
#include "property_adapters.h"

namespace loupe
{
	// The starting point for retrieving reflection data. This cannot be
	// called before main() (during static initialization). However you may
	// still store the result of this function in a globally accessible variable.
	//
	// The `current_program_version` can be used to identify reflection_blobs
	// generated from different compiled version of the program (when loaded from disk).
	[[nodiscard]] reflection_blob reflect(unsigned int current_program_version = 1);

	// Clears the global static task lists, which will reclaim some memory.
	// However, calling reflect() afterwards will produce an empty result.
	void clear_reflect_tasks();
}

#define LOUPE_CONCATENATE(s1, s2) s1##s2
#define LOUPE_CONCATENATE_INDIRECT(s1, s2) LOUPE_CONCATENATE(s1, s2)
#define LOUPE_ANONYMOUS_VARIABLE(str) LOUPE_CONCATENATE_INDIRECT(str, __COUNTER__)

// Allows for the use of complex template types as macro parameters. REFLECT(LOUPE_TEMPLATE(...))
// Normally the ','s in a template parameter list would interfere with macro expansion.
#define LOUPE_TEMPLATE(...) decltype(__VA_ARGS__())

#define REFLECT_SIMPLE(type_name)                                                                                                                                                   \
static_assert(std::is_class_v<type_name> || std::is_enum_v<type_name> || std::is_fundamental_v<type_name>, "Only structs/classes, enums, and fundamental types can be reflected."); \
static_assert(!std::is_const_v<type_name>, "Const types cannot be reflected.");                                                                                                     \
[[maybe_unused]] static const auto& LOUPE_ANONYMOUS_VARIABLE(LOUPE_DUMMY_) =                                                                                                        \
	loupe::detail::get_tasks().emplace_back(loupe::get_type_name<type_name>(), &loupe::detail::init_type_data<std::remove_cv_t<type_name>>, nullptr);

#define REFLECT(type_name)                                                                                                                                                          \
static_assert(std::is_class_v<type_name> || std::is_enum_v<type_name> || std::is_fundamental_v<type_name>, "Only structs/classes, enums, and fundamental types can be reflected."); \
static_assert(!std::is_const_v<type_name>, "Const types cannot be reflected.");                                                                                                     \
[[maybe_unused]] static const auto& LOUPE_ANONYMOUS_VARIABLE(LOUPE_DUMMY_) =                                                                                                        \
	loupe::detail::get_tasks().emplace_back(                                                                                                                                        \
		loupe::get_type_name<type_name>(),                                                                                                                                          \
		&loupe::detail::init_type_data<std::remove_cv_t<type_name>>,                                                                                                                \
		[](loupe::reflection_blob& blob, std::vector<loupe::property>& properties, loupe::type& type, loupe::detail::task_data_stage stage)                                         \
		{                                                                                                                                                                           \
			using reflected_type = std::remove_cv_t<type_name>;

#define ENUM_VALUES           if (stage == loupe::detail::task_data_stage::enums) std::get<loupe::enumeration>(type.data).entries =
#define REF_VALUE(value, ...) loupe::detail::create_enum_entry<__VA_ARGS__>(blob, #value, static_cast<std::size_t>(reflected_type::value)),

#define BASES               if (stage == loupe::detail::task_data_stage::bases) std::get<loupe::structure>(type.data).bases =
#define REF_BASE(base_type) loupe::detail::find_base<base_type, reflected_type>(blob),

#define MEMBERS                                                                            \
	; std::vector<loupe::member>& members = std::get<loupe::structure>(type.data).members; \
	int count = 0;                                                                         \
	loupe::detail::on_scope_exit reserver = [&] { members.reserve(count); };
#define REF_MEMBER(member, ...)                                                                                                                                    \
	++count;                                                                                                                                                       \
	if (stage == loupe::detail::task_data_stage::scan_properties)                                                                                                  \
	{                                                                                                                                                              \
		loupe::detail::register_property<decltype(reflected_type::member)>(blob, properties);                                                                      \
	}                                                                                                                                                              \
	else if (stage == loupe::detail::task_data_stage::members)                                                                                                     \
	{                                                                                                                                                              \
		members.push_back(loupe::detail::create_member<decltype(reflected_type::member), __VA_ARGS__>(blob, #member, __builtin_offsetof(reflected_type, member))); \
	}

#define REF_END ;});

#include "loupe.inl"
