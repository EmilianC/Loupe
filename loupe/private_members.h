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
		REF_MEMBER(bool_value)
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

#define REF_PRIVATE_MEMBER(member, ...) REF_PRIVATE_MEMBER_GET_SET(member, nullptr, nullptr, __VA_ARGS__)
#define REF_PRIVATE_MEMBER_GET_SET(member, getter, setter, ...) {                                                                  \
	++count;                                                                                                                       \
	using Inspector = reflected_type::loupe_reflect_private_##member<void>;                                                        \
	using MemberType = Inspector::MemberType;                                                                                      \
	const std::size_t offset = Inspector::offset;                                                                                  \
	if (stage == loupe::detail::task_data_stage::scan_properties)                                                                  \
	{                                                                                                                              \
		loupe::detail::scan_properties<MemberType>(blob, properties, property_tasks);                                              \
	}                                                                                                                              \
	else if (stage == loupe::detail::task_data_stage::members)                                                                     \
	{                                                                                                                              \
		using GetterType = decltype(getter);                                                                                       \
		using SetterType = decltype(setter);                                                                                       \
		static_assert(std::is_null_pointer_v<GetterType> || std::is_invocable_r_v<MemberType, GetterType, const reflected_type*>,  \
			"Getter should be of the form: `MemberType get() const;`");                                                            \
		static_assert(std::is_null_pointer_v<SetterType> || std::is_invocable_r_v<void, SetterType, reflected_type*, MemberType>,  \
			"Setter should be of the form: `void set(MemberType value);`");                                                        \
		/* Opening a local template context to allow for better constexpr support. */	                                           \
		void* getter_func = []<typename Signature>(Signature) -> void* {                                                           \
			if constexpr (!std::is_null_pointer_v<Signature>)                                                                      \
			{                                                                                                                      \
				return +[](void* base_struct_pointer) -> MemberType {                                                              \
					auto* object = static_cast<const reflected_type*>(base_struct_pointer);                                        \
					auto func = getter;                                                                                            \
					return (object->*func)();                                                                                      \
				};                                                                                                                 \
			} else return nullptr;                                                                                                 \
		}(getter);                                                                                                                 \
                                                                                                                                   \
		void* setter_func = []<typename Signature>(Signature) -> void* {                                                           \
			if constexpr (!std::is_null_pointer_v<Signature>)                                                                      \
			{                                                                                                                      \
				return +[](void* base_struct_pointer, MemberType value) -> void {                                                  \
					auto* object = static_cast<reflected_type*>(base_struct_pointer);                                              \
					auto func = setter;                                                                                            \
					(object->*func)(value);                                                                                        \
				};                                                                                                                 \
			} else return nullptr;                                                                                                 \
		}(setter);                                                                                                                 \
			                                                                                                                       \
		using namespace loupe::metadata;                                                                                           \
		members.push_back(loupe::detail::create_member<MemberType>(                                                                \
			blob, #member, offset, getter_func, setter_func, __VA_ARGS__)                                                          \
		);                                                                                                                         \
	}}
