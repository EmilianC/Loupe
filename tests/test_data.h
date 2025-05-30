#pragma once
#include "loupe/private_members.h"

#include <array>
#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <map>

enum class small_enum : std::uint16_t
{
	local_space = 0,
	world_space = 1,
	COUNT
};

enum class large_enum : std::uint16_t
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
	enum class small_enum : std::uint16_t
	{
		local_space = 0,
		world_space = 1,
		COUNT
	};

	enum class large_enum : std::uint16_t
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
		base_object() { ++count; }
		base_object(std::string new_name) : name(std::move(new_name)) { ++count; }
		~base_object() { --count; }

		std::string name;
		transform world_transform;
		transform local_transform;

		std::array<vec3, 5> previous_positions;
		std::variant<const char*, std::string> description;

		static inline std::size_t count = 0;
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

	void set_health(float new_health) { health = new_health; enabled = health > 0.0f; }

	std::reference_wrapper<mat3> matrix;

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
protected:
	vec3 position;
private:
	quaternion rotation;

public:
	PRIVATE_MEMBER(private_data, position);
	PRIVATE_MEMBER(private_data, rotation);
};
