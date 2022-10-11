#include "test_data.h"
#include "loupe/loupe.h"

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

REFLECT(nested::base_object)
	MEMBERS {
		REF_MEMBER(name)
	}
REF_END;

REFLECT(quaternion)
	MEMBERS {
		REF_MEMBER(x),
		REF_MEMBER(y),
		REF_MEMBER(z),
		REF_MEMBER(w, hidden)
	}
REF_END;

REFLECT(vec3)
	MEMBERS {
		REF_MEMBER(x),
		REF_MEMBER(y),
		REF_MEMBER(z)
	}
REF_END;

REFLECT(mat3)
	MEMBERS {
		REF_MEMBER(data)
	}
REF_END;

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
REF_END;
