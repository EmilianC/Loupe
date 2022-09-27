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


//const auto& dummy = loupe::detail::get_type_descriptor_tasks().emplace_back([](loupe::reflection_blob& blob)
//{
//	loupe::type_descriptor descriptor;
//	descriptor.name = "vec3";
//	descriptor.members = {
//		// macro start
//		{ "x", offsetof(vec3, x) }
//			// metadata macro
//		,
//		{ "y", offsetof(vec3, y) }
//			// metadata macro
//		,
//		{ "z", offsetof(vec3, z) }
//			// metadata macro
//		,
//		// macro end
//	};
//
//	blob.types.push_back(std::move(descriptor));
//});
