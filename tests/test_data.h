#pragma once
#include <array>
#include <string>
#include <variant>
#include <vector>
#include <memory>
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

		std::array<vec3, 5> previous_positions;
		std::variant<const char*, std::string> description;
	};

	struct base_physics
	{
		vec3 velocity;
	};
}

struct game_object : public nested::base_object, public nested::base_physics
{
	bool operator==(const game_object& other) const { return this == &other; }

	float health = 100.0f;
	bool enabled = true;

	std::vector<std::shared_ptr<game_object>> children;
	std::weak_ptr<game_object> parent;
};

struct game_object_group
{
	game_object objects[10];
};

struct world
{
	game_object_group game_objects;
};

template<typename T, typename U>
struct pair
{
	T first;
	U second;
};

struct private_data
{
	//FRIEND_LOUPE;

protected:
	vec3 position;
private:
	quaternion rotation;
};
