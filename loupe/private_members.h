// Copyright (c) 2023 Emilian Cioca
#pragma once

/*
Allows private data members to be seen by the reflection system.

// my_struct.h
struct my_struct
{
	bool bool_value;

private:
	int int_value;
	float float_value;

public: // Must be in a public section.
	PRIVATE_MEMBER(my_struct, int_value);
	PRIVATE_MEMBER(my_struct, float_value);
};

// my_struct.cpp
REFLECT(my_struct)
	MEMBERS {
		REF_MEMBER(bool_value);
		REF_PRIVATE_MEMBER(int_value)
		REF_PRIVATE_MEMBER(float_value)
	}
REF_END;
*/

#define PRIVATE_MEMBER(class_type, member)                                            \
	template<typename DelayedExpansion = void>                                        \
	struct loupe_reflect_private_##member {                                           \
		using MemberType = decltype(class_type::member);                              \
		static constexpr std::size_t offset = __builtin_offsetof(class_type, member); \
	};

#define REF_PRIVATE_MEMBER(member, ...) {                                                                                      \
	++count;                                                                                                                   \
	using Inspector = reflected_type::loupe_reflect_private_##member<void>;                                                    \
	if (stage == loupe::detail::task_data_stage::scan_properties)                                                              \
	{                                                                                                                          \
		loupe::detail::register_property<Inspector::MemberType>(blob, properties);                                             \
	}                                                                                                                          \
	else if (stage == loupe::detail::task_data_stage::members)                                                                 \
	{                                                                                                                          \
		members.push_back(loupe::detail::create_member<Inspector::MemberType, __VA_ARGS__>(blob, #member, Inspector::offset)); \
	}}
