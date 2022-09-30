#include "test_data.h"
#include "loupe.h"

REFLECT_ENUM(small_enum) {
	REF_VALUE(local_space),
	REF_VALUE(world_space),
	REF_VALUE(COUNT)
} REF_ENUM_END;

REFLECT_ENUM(nested::small_enum) {
	REF_VALUE(local_space),
	REF_VALUE(world_space),
	REF_VALUE(COUNT)
} REF_ENUM_END;

REFLECT_ENUM(large_enum) {
	REF_VALUE(value0),
	REF_VALUE(value1),
	REF_VALUE(value2),
	REF_VALUE(value3),
	REF_VALUE(value4),
	REF_VALUE(value5),
	REF_VALUE(value6),
	REF_VALUE(value7),
	REF_VALUE(value8),
	REF_VALUE(value9),
	REF_VALUE(COUNT)
} REF_ENUM_END;

REFLECT_ENUM(nested::large_enum) {
	REF_VALUE(value0),
	REF_VALUE(value1),
	REF_VALUE(value2),
	REF_VALUE(value3),
	REF_VALUE(value4),
	REF_VALUE(value5),
	REF_VALUE(value6),
	REF_VALUE(value7),
	REF_VALUE(value8),
	REF_VALUE(value9),
	REF_VALUE(COUNT)
} REF_ENUM_END;


REFLECT(hidden) REF_END;
REFLECT(editor_only) REF_END;
REFLECT(nested::base_object) REF_END;

REFLECT(quaternion)
	REF_MEMBER(x),
	REF_MEMBER(y),
	REF_MEMBER(z),
	REF_MEMBER(w, hidden)
REF_END;

REFLECT(game_object)
	REF_BASE(nested::base_object) {
	REF_MEMBER(health)
	REF_MEMBER(enabled, editor_only)
} REF_END;


template<typename type, typename... tags>
loupe::member_descriptor register_member(loupe::reflection_blob& blob, const loupe::type_descriptor& owning_type, std::string_view name, std::size_t offset)
{
	loupe::member_descriptor member;
	member.name = name;
	member.offset = offset;
	member.owning_type = &owning_type;

	if constexpr (std::is_enum_v<type>)
	{
		member.enum_type = blob.find_enum<type>();
	}
	else
	{
		member.type = blob.find_type<type>();
	}

	if constexpr (sizeof...(tags) > 0)
	{
		member.metadata.reserve(sizeof...(tags));

		( [&] {
			const loupe::type_descriptor* tag_type = blob.find_type<tags>();
			// error if null.
			member.metadata.push_back(tag_type);
		} (), ...);
	}

	return member;
}

template<typename... bases>
void register_bases(loupe::reflection_blob& blob, loupe::type_descriptor& type)
{
	if constexpr (sizeof...(bases) > 0)
	{
		type.bases.reserve(sizeof...(bases));

		( [&] {
			const loupe::type_descriptor* base_type = blob.find_type<bases>();
			// error if null.
			type.bases.push_back(base_type);
		} (), ...);
	}
}

const auto& dummy = loupe::detail::get_type_descriptor_tasks().emplace_back([](loupe::reflection_blob& blob, loupe::type_descriptor& type, loupe::detail::stage stage)
{
	using class_type = quaternion;

	switch (stage)
	{
	case loupe::detail::stage::types:
		{
			//type = &blob.types.emplace_back();
			type.name = "quaternion";
		}
		break;

	case loupe::detail::stage::bases_and_members:
		{
			register_bases<
				// macro here
			>(blob, type);

			type.members = {
				// find a way to reserve the member count...
				register_member<decltype(class_type::x)>(blob, type, "x", offsetof(class_type, x)),
				register_member<decltype(class_type::y)>(blob, type, "y", offsetof(class_type, y)),
				register_member<decltype(class_type::z)>(blob, type, "z", offsetof(class_type, z)),
				register_member<decltype(class_type::w)>(blob, type, "w", offsetof(class_type, w)),
			};
		}
		break;
	}
});
