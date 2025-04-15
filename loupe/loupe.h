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
		[]([[maybe_unused]] loupe::reflection_blob& blob,                                                                                                                           \
		   [[maybe_unused]] std::vector<loupe::property>& properties,                                                                                                               \
		   [[maybe_unused]] std::vector<loupe::detail::property_task>& property_tasks,                                                                                              \
		   [[maybe_unused]] loupe::type& type,                                                                                                                                      \
		   [[maybe_unused]] loupe::detail::task_data_stage stage)                                                                                                                   \
		{                                                                                                                                                                           \
			using reflected_type = std::remove_cv_t<type_name>;

#define ENUM_VALUES           using namespace loupe::metadata; if (stage == loupe::detail::task_data_stage::enums) std::get<loupe::enumeration>(type.data).entries =
#define REF_VALUE(value, ...) loupe::detail::create_enum_entry(blob, #value, std::to_underlying(reflected_type::value), __VA_ARGS__),

#define BASES                 if (stage == loupe::detail::task_data_stage::bases) std::get<loupe::structure>(type.data).bases =
#define REF_BASE(base_type)   loupe::detail::find_base<base_type, reflected_type>(blob),

#define USER_CONSTRUCTOR(...) ; type.user_constructor = loupe::detail::make_user_constructor<reflected_type, __VA_ARGS__>();

#define MEMBERS                                                                            \
	; std::vector<loupe::member>& members = std::get<loupe::structure>(type.data).members; \
	std::size_t count = 0;                                                                 \
	loupe::detail::on_scope_exit reserver = [&] { members.reserve(count); };

#define REF_MEMBER(member, ...) REF_MEMBER_GET_SET(member, nullptr, nullptr, __VA_ARGS__)
#define REF_MEMBER_GET_SET(member, getter, setter, ...) {                                                                          \
	++count;                                                                                                                       \
	using MemberType = decltype(reflected_type::member);                                                                           \
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
			blob, #member, __builtin_offsetof(reflected_type, member), getter_func, setter_func, __VA_ARGS__)                      \
		);                                                                                                                         \
	}}

#define REF_END ;});

#include "loupe.inl"
