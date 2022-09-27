#include "catch/catch.hpp"
#include "loupe.h"
#include "test_data.h"

TEST_CASE("Reflection Tests")
{
	loupe::reflection_blob ref = loupe::reflect();

	SECTION("Small-Enum")
	{
		const loupe::enum_descriptor* descriptor = ref.find_enum("small_enum");
		REQUIRE(descriptor);
		CHECK(descriptor->name == "small_enum");
		CHECK(descriptor == ref.find<small_enum>());

		REQUIRE(descriptor->entires.size() == 3);
		CHECK(descriptor->entires[0].name == "local_space");
		CHECK(descriptor->entires[0].value == 0);
		CHECK(descriptor->entires[1].name == "world_space");
		CHECK(descriptor->entires[1].value == 1);
		CHECK(descriptor->entires[2].name == "COUNT");
		CHECK(descriptor->entires[2].value == 2);

		SECTION("Nested Namespace")
		{
			const loupe::enum_descriptor* nested_descriptor = ref.find_enum("nested::small_enum");
			REQUIRE(nested_descriptor);
			CHECK(nested_descriptor->name == "nested::small_enum");
			CHECK(nested_descriptor == ref.find<nested::small_enum>());
			CHECK(nested_descriptor != ref.find<small_enum>());
			CHECK(nested_descriptor != descriptor);

			REQUIRE(nested_descriptor->entires.size() == 3);
			CHECK(nested_descriptor->entires[0].name == "local_space");
			CHECK(nested_descriptor->entires[0].value == 0);
			CHECK(nested_descriptor->entires[1].name == "world_space");
			CHECK(nested_descriptor->entires[1].value == 1);
			CHECK(nested_descriptor->entires[2].name == "COUNT");
			CHECK(nested_descriptor->entires[2].value == 2);
		}
	}

	SECTION("Large-Enum")
	{
		const loupe::enum_descriptor* descriptor = ref.find_enum("large_enum");
		REQUIRE(descriptor);
		CHECK(descriptor->name == "large_enum");
		CHECK(descriptor == ref.find<large_enum>());

		REQUIRE(descriptor->entires.size() == 11);
		CHECK(descriptor->entires[0].name == "value0");
		CHECK(descriptor->entires[0].value == 0);
		CHECK(descriptor->entires[1].name == "value1");
		CHECK(descriptor->entires[1].value == 1);
		CHECK(descriptor->entires[2].name == "value2");
		CHECK(descriptor->entires[2].value == 2);
		CHECK(descriptor->entires[3].name == "value3");
		CHECK(descriptor->entires[3].value == 3);
		CHECK(descriptor->entires[4].name == "value4");
		CHECK(descriptor->entires[4].value == 4);
		CHECK(descriptor->entires[5].name == "value5");
		CHECK(descriptor->entires[5].value == 5);
		CHECK(descriptor->entires[6].name == "value6");
		CHECK(descriptor->entires[6].value == 6);
		CHECK(descriptor->entires[7].name == "value7");
		CHECK(descriptor->entires[7].value == 7);
		CHECK(descriptor->entires[8].name == "value8");
		CHECK(descriptor->entires[8].value == 8);
		CHECK(descriptor->entires[9].name == "value9");
		CHECK(descriptor->entires[9].value == 9);
		CHECK(descriptor->entires[10].name == "COUNT");
		CHECK(descriptor->entires[10].value == 10);

		SECTION("Nested Namespace")
		{
			const loupe::enum_descriptor* nested_descriptor = ref.find_enum("nested::large_enum");
			REQUIRE(nested_descriptor);
			CHECK(nested_descriptor->name == "nested::large_enum");
			CHECK(nested_descriptor == ref.find<nested::large_enum>());
			CHECK(nested_descriptor != ref.find<large_enum>());
			CHECK(nested_descriptor != descriptor);

			REQUIRE(nested_descriptor->entires.size() == 11);
			CHECK(nested_descriptor->entires[0].name == "value0");
			CHECK(nested_descriptor->entires[0].value == 0);
			CHECK(nested_descriptor->entires[1].name == "value1");
			CHECK(nested_descriptor->entires[1].value == 1);
			CHECK(nested_descriptor->entires[2].name == "value2");
			CHECK(nested_descriptor->entires[2].value == 2);
			CHECK(nested_descriptor->entires[3].name == "value3");
			CHECK(nested_descriptor->entires[3].value == 3);
			CHECK(nested_descriptor->entires[4].name == "value4");
			CHECK(nested_descriptor->entires[4].value == 4);
			CHECK(nested_descriptor->entires[5].name == "value5");
			CHECK(nested_descriptor->entires[5].value == 5);
			CHECK(nested_descriptor->entires[6].name == "value6");
			CHECK(nested_descriptor->entires[6].value == 6);
			CHECK(nested_descriptor->entires[7].name == "value7");
			CHECK(nested_descriptor->entires[7].value == 7);
			CHECK(nested_descriptor->entires[8].name == "value8");
			CHECK(nested_descriptor->entires[8].value == 8);
			CHECK(nested_descriptor->entires[9].name == "value9");
			CHECK(nested_descriptor->entires[9].value == 9);
			CHECK(nested_descriptor->entires[10].name == "COUNT");
			CHECK(nested_descriptor->entires[10].value == 10);
		}
	}
}
