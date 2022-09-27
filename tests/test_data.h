#pragma once

enum class small_enum : short
{
	local_space = 0,
	world_space = 1,
	COUNT
};

enum class large_enum : int
{
	value0,
	value1,
	value2,
	value3,
	value4,
	value5,
	value6,
	value7,
	value8,
	value9,
	COUNT
};

namespace nested
{
	enum class small_enum : short
	{
		local_space = 0,
		world_space = 1,
		COUNT
	};

	enum class large_enum : int
	{
		value0,
		value1,
		value2,
		value3,
		value4,
		value5,
		value6,
		value7,
		value8,
		value9,
		COUNT
	};

	struct vec3
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
}

struct quaternion
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;
};

struct transform
{
	nested::vec3 position;
	nested::vec3 scale;
	quaternion rotation;
	small_enum space = small_enum::local_space;
	nested::large_enum value = nested::large_enum::value2;
};

