// Copyright (c) 2023 Emilian Cioca
#pragma once

// This macro is provided separately to allow for lightweight inclusion
// in headers, instead of including loupe.h and all its dependencies.

// Exposes a private member to the reflection system from the class or
// structure declaration. See loupe.h for usage details.
#define PRIVATE_MEMBER(class_type, member)                                     \
	using _loupe_reflect_private_type_##member = decltype(class_type::member); \
	static consteval std::size_t _loupe_reflect_private_offset_##member()      \
	{                                                                          \
		return __builtin_offsetof(class_type, member);                         \
	};
