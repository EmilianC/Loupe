#include "test_data.h"
#include "loupe/loupe.h"

namespace nested
{
	unsigned base_object::num_objects = 0;
}

// This is reflected without macros just to make it easier to iterate in development.
static_assert(!std::is_reference_v<quaternion>, "References cannot be reflected.");
[[maybe_unused]] static const auto& manually_expanded =
loupe::detail::get_tasks().emplace_back(loupe::get_type_name<quaternion>(), [](loupe::reflection_blob& blob, loupe::type& type, loupe::detail::task_stage stage)
{
	using reflected_type = std::remove_cv_t<quaternion>;
	switch (stage)
	{
	case loupe::detail::task_stage::type_adapters:
		if constexpr (std::is_void_v<reflected_type>)
		{
			type.data = loupe::fundamental_type{};
			type.size = 0;
			type.alignment = 1;
		}
		else
		{
			type.size = sizeof(reflected_type);
			type.alignment = alignof(reflected_type);
			if constexpr (std::is_default_constructible_v<reflected_type>)
			{
				type.construct = []() { return std::make_any<reflected_type>(); };
				type.construct_at = [](void* location) {
					LOUPE_ASSERT(reinterpret_cast<std::uintptr_t>(location) % alignof(reflected_type) != 0, "Construction location for type is misaligned.");
					new (location) reflected_type;
				};
			}
			if constexpr (loupe::array_adapter<reflected_type>::value)
				type.data = loupe::array_adapter<reflected_type>::make_data(blob);
			else if constexpr (loupe::enum_adapter<reflected_type>::value)
				type.data = loupe::enum_adapter<reflected_type>::make_data(blob);
			else if constexpr (std::is_class_v<reflected_type>)
				type.data = loupe::class_type{};
			else if constexpr (std::is_fundamental_v<reflected_type>)
				type.data = loupe::fundamental_type{};
			else LOUPE_ASSERT(false, "Unrecognized type category.");
		}
		break;
	case loupe::detail::task_stage::enums_bases_members:
		// Manual macro expansion again.
		; std::get<loupe::class_type>(type.data).variables = {
			loupe::detail::register_variable<decltype(reflected_type::x)>(blob, "x", offsetof(reflected_type, x)),
			loupe::detail::register_variable<decltype(reflected_type::y)>(blob, "y", offsetof(reflected_type, y)),
			loupe::detail::register_variable<decltype(reflected_type::z)>(blob, "z", offsetof(reflected_type, z)),
			loupe::detail::register_variable<decltype(reflected_type::w)>(blob, "w", offsetof(reflected_type, w))
		}
		;
	}
});

REFLECT(hidden) REF_END;
REFLECT(editor_only) REF_END;

REFLECT(small_enum) ENUM_VALUES {
	REF_VALUE(local_space),
	REF_VALUE(world_space),
	REF_VALUE(COUNT, hidden)
} REF_END;

REFLECT(nested::small_enum) ENUM_VALUES {
	REF_VALUE(local_space),
	REF_VALUE(world_space),
	REF_VALUE(COUNT, hidden)
} REF_END;

REFLECT(large_enum) ENUM_VALUES {
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
	REF_VALUE(COUNT, hidden)
} REF_END;

REFLECT(nested::large_enum) ENUM_VALUES {
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
	REF_VALUE(COUNT, hidden)
} REF_END;

REFLECT(nested::base_object)
	MEMBERS {
		REF_MEMBER(name)
	}
	STATIC_MEMBERS {
		REF_STATIC_MEMBER(num_objects)
	}
REF_END;

REFLECT(vec3) MEMBERS {
	REF_MEMBER(x),
	REF_MEMBER(y),
	REF_MEMBER(z)
} REF_END;

REFLECT(float[9]) REF_END;
REFLECT(mat3) MEMBERS {
	REF_MEMBER(data)
} REF_END;

REFLECT(game_object)
	BASES {
		REF_BASE(nested::base_object)
	}
	MEMBERS {
		REF_MEMBER(health),
		REF_MEMBER(enabled, editor_only)
	}
REF_END;

REFLECT(container<int>)
	MEMBERS {
		REF_MEMBER(value)
	}
	STATIC_MEMBERS {
		REF_STATIC_MEMBER(static_value)
	}
REF_END;

REFLECT(std::vector<game_object>) REF_END;
REFLECT(world) MEMBERS {
	REF_MEMBER(game_objects)
} REF_END;
