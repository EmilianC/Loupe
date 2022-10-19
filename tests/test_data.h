#pragma once
#include <string>
#include <vector>
#include <map>

struct hidden {};
struct editor_only {};

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

struct quaternion
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;
};

struct vec3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

struct mat3
{
	float data[9] = { 0.0f };
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

	struct transform
	{
		vec3 position;
		vec3 scale;
		quaternion rotation;
		small_enum space = small_enum::local_space;
		nested::large_enum value = nested::large_enum::value2;
	};

	struct base_object
	{
		std::string name;
		transform world_transform;
		transform local_transform;

		static unsigned num_objects;
	};

	struct base_physics
	{
		vec3 velocity;
	};
}

struct game_object : public nested::base_object, public nested::base_physics
{
	float health = 100.0f;
	bool enabled = true;
};

struct world
{
	std::vector<game_object> game_objects;
};

template<typename T>
struct container
{
	T value;
	static inline T static_value;
};

struct private_data
{
	//FRIEND_LOUPE;

protected:
	vec3 position;
private:
	quaternion rotation;
};
