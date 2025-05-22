#include "catch/catch.hpp"
#include "loupe/loupe.h"
#include "test_data.h"

TEST_CASE("Reflection Tests - Structures")
{
	loupe::reflection_blob ref = loupe::reflect();

	const loupe::type* game_object_descriptor = ref.find("game_object");
	REQUIRE(game_object_descriptor);

	SECTION("Bases")
	{
		const auto& structure = std::get<loupe::structure>(game_object_descriptor->data);

		REQUIRE(structure.bases.size() == 2);
		CHECK(structure.bases[0].offset == 0);
		CHECK(structure.bases[1].offset == sizeof(nested::base_object));
	}
}
