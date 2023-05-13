// Copyright (c) 2022 Emilian Cioca
#pragma once
#include "assert.h"
#include "type_adapters.h"

namespace loupe
{
	// The starting point for retrieving reflection data. This cannot be
	// called before main() (during static initialization). You may still
	// store this result statically/globally in your program at the top
	// of your main() function, or whenever you wish afterwards.
	[[nodiscard]] reflection_blob reflect();

	// Clears the global static task lists, which will reclaim some memory.
	// However, calling reflect() afterwards will result in an empty result.
	void clear_reflect_tasks();
}

#define LOUPE_CONCATENATE(s1, s2) s1##s2
#define LOUPE_CONCATENATE_INDIRECT(s1, s2) LOUPE_CONCATENATE(s1, s2)
#define LOUPE_ANONYMOUS_VARIABLE(str) LOUPE_CONCATENATE_INDIRECT(str, __COUNTER__)

// Allows for the use of complex template types as macro parameters.
// Normally the ','s in a template parameter list would interfere with the macro expansion.
#define LOUPE_TEMPLATE(...) decltype(__VA_ARGS__())

#define LOUPE_FRIEND template<typename T, unsigned> friend struct offset_collector;

#define REFLECT(type_name)                                                                                                                                      \
static_assert(!std::is_reference_v<type_name>, "References cannot be reflected.");                                                                              \
[[maybe_unused]] static const auto& LOUPE_ANONYMOUS_VARIABLE(LOUPE_DUMMY_) =                                                                                    \
loupe::detail::get_tasks().emplace_back(loupe::get_type_name<type_name>(), [](loupe::reflection_blob& blob, loupe::type& type, loupe::detail::task_stage stage) \
{                                                                                                                                                               \
	using reflected_type = std::remove_cv_t<type_name>;                                                                                                         \
	switch (stage)                                                                                                                                              \
	{                                                                                                                                                           \
	case loupe::detail::task_stage::type_adapters:                                                                                                              \
		if constexpr (std::is_void_v<reflected_type>)                                                                                                           \
		{                                                                                                                                                       \
			type.data = loupe::fundamental{};                                                                                                                   \
			type.size = 0;                                                                                                                                      \
			type.alignment = 1;                                                                                                                                 \
		}                                                                                                                                                       \
		else                                                                                                                                                    \
		{                                                                                                                                                       \
			type.size = sizeof(reflected_type);                                                                                                                 \
			type.alignment = alignof(reflected_type);                                                                                                           \
			if constexpr (std::is_default_constructible_v<reflected_type>)                                                                                      \
			{                                                                                                                                                   \
				type.construct_at = [](void* location) {                                                                                                        \
					LOUPE_ASSERT(reinterpret_cast<std::uintptr_t>(location) % alignof(reflected_type) != 0, "Construction location for type is misaligned.");   \
					new (location) reflected_type;                                                                                                              \
				};                                                                                                                                              \
				if constexpr (std::is_trivially_copyable_v<reflected_type>)                                                                                     \
				{                                                                                                                                               \
					type.construct = []() { return std::make_any<reflected_type>(); };                                                                          \
				}                                                                                                                                               \
			}                                                                                                                                                   \
			if constexpr (loupe::pointer_adapter<reflected_type>::value)                                                                                        \
				type.data = loupe::pointer_adapter<reflected_type>::make_data(blob);                                                                            \
			else if constexpr (loupe::array_adapter<reflected_type>::value)                                                                                     \
				type.data = loupe::array_adapter<reflected_type>::make_data(blob);                                                                              \
			else if constexpr (loupe::enum_adapter<reflected_type>::value)                                                                                      \
				type.data = loupe::enum_adapter<reflected_type>::make_data(blob);                                                                               \
			else if constexpr (std::is_class_v<reflected_type>)                                                                                                 \
				type.data = loupe::structure{};                                                                                                                 \
			else if constexpr (std::is_fundamental_v<reflected_type>)                                                                                           \
				type.data = loupe::fundamental{};                                                                                                               \
			else LOUPE_ASSERT(false, "Unsupported type category.");                                                                                             \
		}                                                                                                                                                       \
		break;                                                                                                                                                  \
	case loupe::detail::task_stage::enums_bases_members:

#define ENUM_VALUES           ; std::get<loupe::enumeration>(type.data).entries =
#define REF_VALUE(value, ...) loupe::detail::register_enum_entry<__VA_ARGS__>(blob, #value, static_cast<std::size_t>(reflected_type::value))

#define BASES               ; std::get<loupe::structure>(type.data).bases =
#define REF_BASE(base_type) loupe::detail::register_base<base_type>(blob),

#define MEMBERS                 ; std::get<loupe::structure>(type.data).members =
#define REF_MEMBER(member, ...) loupe::detail::register_member<decltype(reflected_type::member), __VA_ARGS__>(blob, #member, offsetof(reflected_type, member))

#define STATIC_MEMBERS                 ; std::get<loupe::structure>(type.data).static_members =
#define REF_STATIC_MEMBER(member, ...) loupe::detail::register_static_member<decltype(reflected_type::member), __VA_ARGS__>(blob, #member, &reflected_type::member)

#define REF_END ;}});

#include "loupe.inl"
