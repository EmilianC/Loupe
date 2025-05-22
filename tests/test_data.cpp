#include "test_data.h"
#include "loupe/loupe.h"
#include "loupe/metadata.h"
#include "loupe/property_adapters.h"
#include <array>

namespace loupe::adapters
{
	// Custom array adapter for game_object_groups.
	template<>
	struct array_adapter<game_object_group> : public std::true_type
	{
		using ArrayType = game_object_group;
		using ElementType = game_object;
		static constexpr std::size_t ArraySize = sizeof(game_object_group::objects) / sizeof(game_object);

		[[nodiscard]] static array make_data(const reflection_blob& blob)
		{
			const property* element_property = blob.find_property<ElementType>();
			LOUPE_ASSERT(element_property, "The array's element type was not registered. It must be reflected separately.");

			return {
				.element_property = element_property,
				.dynamic = false,
				.unique_set = false,
				.get_count = [](const void*) -> std::size_t { return ArraySize; },

				.contains = [](const void* array, const void* data) -> bool {
					auto* container = static_cast<const ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					return std::find(std::begin(container->objects), std::end(container->objects), *element) != std::end(container->objects);
				},

				.get_at = [](const void* array, std::size_t index) -> const void* {
					LOUPE_ASSERT(index < ArraySize, "Index out of bounds.");
					auto* container = static_cast<const ArrayType*>(array);
					return &container->objects[index];
				},

				.set_at = [](void* array, std::size_t index, const void* data) {
					LOUPE_ASSERT(index < ArraySize, "Index out of bounds.");
					auto* container = static_cast<ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);

					container->objects[index] = *element;
				}
			};
		}
	};
}

REFLECT(small_enum) ENUM_VALUES {
	REF_VALUE(local_space)
	REF_VALUE(world_space)
	REF_VALUE(COUNT, hidden())
} REF_END;

REFLECT(nested::small_enum) ENUM_VALUES {
	REF_VALUE(local_space)
	REF_VALUE(world_space)
	REF_VALUE(COUNT, hidden())
} REF_END;

REFLECT(large_enum) ENUM_VALUES {
	REF_VALUE(value0)
	REF_VALUE(value1)
	REF_VALUE(value2)
	REF_VALUE(value3)
	REF_VALUE(value4)
	REF_VALUE(value5)
	REF_VALUE(value6)
	REF_VALUE(value7)
	REF_VALUE(value8)
	REF_VALUE(value9)
	REF_VALUE(COUNT, hidden())
} REF_END;

REFLECT(nested::large_enum) ENUM_VALUES {
	REF_VALUE(value0)
	REF_VALUE(value1)
	REF_VALUE(value2)
	REF_VALUE(value3)
	REF_VALUE(value4)
	REF_VALUE(value5)
	REF_VALUE(value6)
	REF_VALUE(value7)
	REF_VALUE(value8)
	REF_VALUE(value9)
	REF_VALUE(COUNT, hidden())
} REF_END;

REFLECT(nested::transform) MEMBERS {
	REF_MEMBER(position)
	REF_MEMBER(scale)
	REF_MEMBER(rotation)
	REF_MEMBER(space)
	REF_MEMBER(value)
} REF_END;

REFLECT(nested::base_object)
	USER_CONSTRUCTOR(std::string)
	MEMBERS {
		REF_MEMBER(name)
		REF_MEMBER(world_transform)
		REF_MEMBER(local_transform)
		REF_MEMBER(previous_positions)
		REF_MEMBER(description)
	}
REF_END;

REFLECT(nested::base_physics)
	MEMBERS {
		REF_MEMBER(velocity)
	}
REF_END;


REFLECT(quaternion) MEMBERS {
	REF_MEMBER(x)
	REF_MEMBER(y)
	REF_MEMBER(z)
	REF_MEMBER(w, hidden())
} REF_END;

REFLECT(vec3) MEMBERS {
	REF_MEMBER(x)
	REF_MEMBER(y)
	REF_MEMBER(z)
} REF_END;

REFLECT(mat3) MEMBERS {
	REF_MEMBER(data)
} REF_END;

REFLECT(game_object)
	BASES {
		REF_BASE(nested::base_object)
		REF_BASE(nested::base_physics)
	}
	MEMBERS {
		REF_MEMBER(health, range(0.0f, 100.0f))
		REF_MEMBER(enabled)
		REF_MEMBER(matrix)
		REF_MEMBER(children)
		REF_MEMBER(parent)
	}
REF_END;

REFLECT(game_object_group) MEMBERS {
	REF_MEMBER(objects)
} REF_END;

REFLECT(world) MEMBERS {
	REF_MEMBER(game_objects)
} REF_END;

REFLECT(LOUPE_TEMPLATE(pair<int, float>)) MEMBERS {
	REF_MEMBER(first)
	REF_MEMBER(second)
} REF_END;
