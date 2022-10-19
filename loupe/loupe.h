// Copyright (c) 2022 Emilian Cioca
#pragma once
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


/* Reflect private variable example
*
#include <array>
#include <string_view>
#include <type_traits>
#include <cstddef>

struct test
{
	template<typename T, unsigned> friend struct offset_collector; // this needs to be a macro
private:
	int buffer[100];
	int temp;
};

// this needs to be a macro as well, REFLECT_PRIVATE() it'll need to be outside the usual reflection block.
template<typename T, unsigned size>
struct offset_collector2
{
	using type = T;
	static constexpr unsigned offset = size;
};

template<typename T, unsigned>
struct offset_collector : public offset_collector2<decltype(test::temp), offsetof(test, temp)>
{
	using type2 = offset_collector2::type;
	static constexpr unsigned offset2 = offset_collector2::offset;
};

int main()
{
	return offset_collector<int, 0>::offset2;
}

//lastly, we'll need a PRIVATE_MEMBER() macro in the main block which knows to look for the temp collector types instead of calling offsetof()
*/


#define LOUPE_CONCATENATE(s1, s2) s1##s2
#define LOUPE_CONCATENATE_INDIRECT(s1, s2) LOUPE_CONCATENATE(s1, s2)
#define LOUPE_ANONYMOUS_VARIABLE(str) LOUPE_CONCATENATE_INDIRECT(str, __COUNTER__)

// Allows for the use of complex template types as macro parameters.
// Normally the ','s in a template parameter list would interfere with the macro expansion.
#define LOUPE_TEMPLATE(...) decltype(__VA_ARGS__())

#define LOUPE_FRIEND template<typename T, unsigned> friend struct offset_collector;

#define REFLECT(type_name)                                                                                                                                      \
[[maybe_unused]] static const auto& LOUPE_ANONYMOUS_VARIABLE(LOUPE_DUMMY_) =                                                                                    \
loupe::detail::get_tasks().emplace_back(loupe::get_type_name<type_name>(), [](loupe::reflection_blob& blob, loupe::type& type, loupe::detail::task_stage stage) \
{                                                                                                                                                               \
	using reflected_type = type_name;                                                                                                                           \
	switch (stage)                                                                                                                                              \
	{                                                                                                                                                           \
	case loupe::detail::task_stage::type_adapters:                                                                                                              \
		if constexpr (std::is_void_v<reflected_type>)                                                                                                           \
		{                                                                                                                                                       \
			type.data = loupe::void_type{};                                                                                                                     \
			type.size = 0;                                                                                                                                      \
			type.alignment = 1;                                                                                                                                 \
		}                                                                                                                                                       \
		else                                                                                                                                                    \
		{                                                                                                                                                       \
			type.size = sizeof(reflected_type);                                                                                                                 \
			type.alignment = alignof(reflected_type);                                                                                                           \
			if constexpr (std::is_default_constructible_v<reflected_type>)                                                                                      \
			{                                                                                                                                                   \
				type.default_constructible = true;                                                                                                              \
				type.construct_implementation = []() { return std::make_any<reflected_type>(); };                                                               \
				type.construct_at_implementation = [](void* ptr) { new (ptr) reflected_type; };                                                                 \
			}                                                                                                                                                   \
			else                                                                                                                                                \
			{                                                                                                                                                   \
				type.default_constructible = false;                                                                                                             \
			}                                                                                                                                                   \
			if constexpr (loupe::array_adapter<reflected_type>::value)                                                                                          \
				type.data = loupe::array_adapter<reflected_type>::make_data(blob);                                                                              \
			else if constexpr (loupe::enum_adapter<reflected_type>::value)                                                                                      \
				type.data = loupe::enum_adapter<reflected_type>::make_data(blob);                                                                               \
			else                                                                                                                                                \
			{                                                                                                                                                   \
				type.data = loupe::class_type{};                                                                                                                \
			}                                                                                                                                                   \
		}                                                                                                                                                       \
		break;                                                                                                                                                  \
	case loupe::detail::task_stage::enums_bases_members:

#define ENUM_VALUES           ; std::get<loupe::enum_type>(type.data).entries =
#define REF_VALUE(value, ...) loupe::detail::register_enum_entry<__VA_ARGS__>(blob, #value, static_cast<std::size_t>(reflected_type::value))

#define BASES               ; std::get<loupe::class_type>(type.data).bases =
#define REF_BASE(base_type) loupe::detail::register_base<base_type>(blob),

#define MEMBERS                 ; std::get<loupe::class_type>(type.data).variables =
#define REF_MEMBER(member, ...) loupe::detail::register_variable<decltype(reflected_type::member), __VA_ARGS__>(blob, #member, offsetof(reflected_type, member))

#define STATIC_MEMBERS                 ; std::get<loupe::class_type>(type.data).static_variables =
#define REF_STATIC_MEMBER(member, ...) loupe::detail::register_static_variable<decltype(reflected_type::member), __VA_ARGS__>(blob, #member, &reflected_type::member)

#define REF_END ;}});

#include "loupe.inl"
