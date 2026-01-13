// Copyright (c) 2022 Emilian Cioca
#pragma once
#include "assert.h"
#include "core_data.h"
#include "private_members.h"
#include "property_adapters.h"

#include <bit>
#include <functional>

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

#include "loupe.inl"

/*
The macros defined bellow allow you to build up your reflection structures by opening
a reflection block and specifying the various properties of your types.
This must be done in .cpp files, so that they are not compiled multiple times.

Each section of a reflection block is optional and they can be formatted freely.

struct example_type : public base1, public base2
{
	example_type(int, float);
private:
	int _internal;
public:
	float memberA;
	float memberB;
	float memberC;
	float memberD;

	float get_memberD() const;
	void set_memberD(float);

	// Required to expose the private member to the reflection system.
	PRIVATE_MEMBER(my_struct, _internal);
};

REFLECT(example_type)
	USER_CONSTRUCTOR(int, float)
	BASES {
		REF_BASE(base1)
		REF_BASE(base2)
	}
	MEMBERS {
		REF_MEMBER(_internal)
		REF_MEMBER(memberA, display_name("My Value"), range(0.0f, 1.0f))
		REF_MEMBER(memberB, readonly())
		REF_MEMBER(memberC)
		REF_MEMBER_EX(memberD,
			&example_type::get_memberD,
			&example_type::set_memberD,
			not_serialized()
		)
	}
REF_END;

enum class example_enum : std::uint16_t
{
	value0, value1, COUNT
};

REFLECT(example_enum)
	ENUM_VALUES {
		REF_VALUE(value0)
		REF_VALUE(value1)
		REF_VALUE(COUNT, hidden())
	}
REF_END;
*/

// Quickly expose a type to the reflection system without opening a reflection block.
#define REFLECT_SIMPLE(type_name) LOUPE_REFLECT_SIMPLE(type_name)

// Opens a reflection block to describe the given type.
#define REFLECT(type_name) LOUPE_REFLECT(type_name)

	// Reflect enumerations and their values.
	// Metadata can be specified per-value.
	#define ENUM_VALUES LOUPE_ENUM_VALUES
		#define REF_VALUE(value, ...) LOUPE_REF_VALUE(value, __VA_ARGS__)

	// Reflect the signature of a user-defined constructor.
	// The constructor can be called dynamically later via `type::user_construct_at()`.
	#define USER_CONSTRUCTOR(...)     LOUPE_USER_CONSTRUCTOR(__VA_ARGS__)

	// Reflect base classes/structures of a type.
	#define BASES LOUPE_BASES
		#define REF_BASE(base_type)   LOUPE_REF_BASE(base_type)

	// Reflect members of a class or structure. Any reflected getters and setters will
	// automatically be used by `member::get_copy_from()` and `member::set_on()`.
	// Metadata can be specified per-member.
	#define MEMBERS LOUPE_MEMBERS
		#define REF_MEMBER(member, ...)                    LOUPE_MEMBER_EX(member, nullptr, nullptr, __VA_ARGS__)
		#define REF_MEMBER_EX(member, getter, setter, ...) LOUPE_MEMBER_EX(member, getter,  setter,  __VA_ARGS__)

// Terminates a reflection block.
#define REF_END LOUPE_REF_END

// Allows for the use of complex template types as macro parameters. Normally the
// ','s in a template parameter list would interfere with macro expansion.
// For example:
//     `REF_BASE(LOUPE_TEMPLATE(base<int, float>))`
#define LOUPE_TEMPLATE(...) decltype(__VA_ARGS__())
