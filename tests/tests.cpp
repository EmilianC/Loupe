#include "catch/catch.hpp"
#include "loupe.h"
#include "test_data.h"

TEST_CASE("Reflection Tests")
{
	loupe::reflection_blob ref = loupe::reflect();

	SECTION("Enums")
	{
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

			REQUIRE(descriptor->find_name_from_value(0));
			REQUIRE(descriptor->find_name_from_value(1));
			REQUIRE(descriptor->find_name_from_value(2));
			CHECK(*descriptor->find_name_from_value(0) == "local_space");
			CHECK(*descriptor->find_name_from_value(1) == "world_space");
			CHECK(*descriptor->find_name_from_value(2) == "COUNT");
			CHECK(descriptor->find_name_from_value(3) == nullptr);

			REQUIRE(descriptor->find_value_from_name("local_space"));
			REQUIRE(descriptor->find_value_from_name("world_space"));
			REQUIRE(descriptor->find_value_from_name("COUNT"));
			CHECK(*descriptor->find_value_from_name("local_space") == 0);
			CHECK(*descriptor->find_value_from_name("world_space") == 1);
			CHECK(*descriptor->find_value_from_name("COUNT") == 2);
			CHECK(descriptor->find_value_from_name("local") == nullptr);

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

				REQUIRE(nested_descriptor->find_name_from_value(0));
				REQUIRE(nested_descriptor->find_name_from_value(1));
				REQUIRE(nested_descriptor->find_name_from_value(2));
				CHECK(*nested_descriptor->find_name_from_value(0) == "local_space");
				CHECK(*nested_descriptor->find_name_from_value(1) == "world_space");
				CHECK(*nested_descriptor->find_name_from_value(2) == "COUNT");
				CHECK(nested_descriptor->find_name_from_value(3) == nullptr);

				REQUIRE(nested_descriptor->find_value_from_name("local_space"));
				REQUIRE(nested_descriptor->find_value_from_name("world_space"));
				REQUIRE(nested_descriptor->find_value_from_name("COUNT"));
				CHECK(*nested_descriptor->find_value_from_name("local_space") == 0);
				CHECK(*nested_descriptor->find_value_from_name("world_space") == 1);
				CHECK(*nested_descriptor->find_value_from_name("COUNT") == 2);
				CHECK(nested_descriptor->find_value_from_name("local") == nullptr);
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

			REQUIRE(descriptor->find_name_from_value(0));
			REQUIRE(descriptor->find_name_from_value(1));
			REQUIRE(descriptor->find_name_from_value(2));
			REQUIRE(descriptor->find_name_from_value(3));
			REQUIRE(descriptor->find_name_from_value(4));
			REQUIRE(descriptor->find_name_from_value(5));
			REQUIRE(descriptor->find_name_from_value(6));
			REQUIRE(descriptor->find_name_from_value(7));
			REQUIRE(descriptor->find_name_from_value(8));
			REQUIRE(descriptor->find_name_from_value(9));
			REQUIRE(descriptor->find_name_from_value(10));
			CHECK(*descriptor->find_name_from_value(0) == "value0");
			CHECK(*descriptor->find_name_from_value(1) == "value1");
			CHECK(*descriptor->find_name_from_value(2) == "value2");
			CHECK(*descriptor->find_name_from_value(3) == "value3");
			CHECK(*descriptor->find_name_from_value(4) == "value4");
			CHECK(*descriptor->find_name_from_value(5) == "value5");
			CHECK(*descriptor->find_name_from_value(6) == "value6");
			CHECK(*descriptor->find_name_from_value(7) == "value7");
			CHECK(*descriptor->find_name_from_value(8) == "value8");
			CHECK(*descriptor->find_name_from_value(9) == "value9");
			CHECK(*descriptor->find_name_from_value(10) == "COUNT");
			CHECK(descriptor->find_name_from_value(11) == nullptr);

			REQUIRE(descriptor->find_value_from_name("value0"));
			REQUIRE(descriptor->find_value_from_name("value1"));
			REQUIRE(descriptor->find_value_from_name("value2"));
			REQUIRE(descriptor->find_value_from_name("value3"));
			REQUIRE(descriptor->find_value_from_name("value4"));
			REQUIRE(descriptor->find_value_from_name("value5"));
			REQUIRE(descriptor->find_value_from_name("value6"));
			REQUIRE(descriptor->find_value_from_name("value7"));
			REQUIRE(descriptor->find_value_from_name("value8"));
			REQUIRE(descriptor->find_value_from_name("value9"));
			REQUIRE(descriptor->find_value_from_name("COUNT"));
			CHECK(*descriptor->find_value_from_name("value0") == 0);
			CHECK(*descriptor->find_value_from_name("value1") == 1);
			CHECK(*descriptor->find_value_from_name("value2") == 2);
			CHECK(*descriptor->find_value_from_name("value3") == 3);
			CHECK(*descriptor->find_value_from_name("value4") == 4);
			CHECK(*descriptor->find_value_from_name("value5") == 5);
			CHECK(*descriptor->find_value_from_name("value6") == 6);
			CHECK(*descriptor->find_value_from_name("value7") == 7);
			CHECK(*descriptor->find_value_from_name("value8") == 8);
			CHECK(*descriptor->find_value_from_name("value9") == 9);
			CHECK(*descriptor->find_value_from_name("COUNT") == 10);
			CHECK(descriptor->find_value_from_name("count") == nullptr);

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

				REQUIRE(descriptor->find_name_from_value(0));
				REQUIRE(descriptor->find_name_from_value(1));
				REQUIRE(descriptor->find_name_from_value(2));
				REQUIRE(descriptor->find_name_from_value(3));
				REQUIRE(descriptor->find_name_from_value(4));
				REQUIRE(descriptor->find_name_from_value(5));
				REQUIRE(descriptor->find_name_from_value(6));
				REQUIRE(descriptor->find_name_from_value(7));
				REQUIRE(descriptor->find_name_from_value(8));
				REQUIRE(descriptor->find_name_from_value(9));
				REQUIRE(descriptor->find_name_from_value(10));
				CHECK(*descriptor->find_name_from_value(0) == "value0");
				CHECK(*descriptor->find_name_from_value(1) == "value1");
				CHECK(*descriptor->find_name_from_value(2) == "value2");
				CHECK(*descriptor->find_name_from_value(3) == "value3");
				CHECK(*descriptor->find_name_from_value(4) == "value4");
				CHECK(*descriptor->find_name_from_value(5) == "value5");
				CHECK(*descriptor->find_name_from_value(6) == "value6");
				CHECK(*descriptor->find_name_from_value(7) == "value7");
				CHECK(*descriptor->find_name_from_value(8) == "value8");
				CHECK(*descriptor->find_name_from_value(9) == "value9");
				CHECK(*descriptor->find_name_from_value(10) == "COUNT");
				CHECK(descriptor->find_name_from_value(11) == nullptr);

				REQUIRE(descriptor->find_value_from_name("value0"));
				REQUIRE(descriptor->find_value_from_name("value1"));
				REQUIRE(descriptor->find_value_from_name("value2"));
				REQUIRE(descriptor->find_value_from_name("value3"));
				REQUIRE(descriptor->find_value_from_name("value4"));
				REQUIRE(descriptor->find_value_from_name("value5"));
				REQUIRE(descriptor->find_value_from_name("value6"));
				REQUIRE(descriptor->find_value_from_name("value7"));
				REQUIRE(descriptor->find_value_from_name("value8"));
				REQUIRE(descriptor->find_value_from_name("value9"));
				REQUIRE(descriptor->find_value_from_name("COUNT"));
				CHECK(*descriptor->find_value_from_name("value0") == 0);
				CHECK(*descriptor->find_value_from_name("value1") == 1);
				CHECK(*descriptor->find_value_from_name("value2") == 2);
				CHECK(*descriptor->find_value_from_name("value3") == 3);
				CHECK(*descriptor->find_value_from_name("value4") == 4);
				CHECK(*descriptor->find_value_from_name("value5") == 5);
				CHECK(*descriptor->find_value_from_name("value6") == 6);
				CHECK(*descriptor->find_value_from_name("value7") == 7);
				CHECK(*descriptor->find_value_from_name("value8") == 8);
				CHECK(*descriptor->find_value_from_name("value9") == 9);
				CHECK(*descriptor->find_value_from_name("COUNT") == 10);
				CHECK(descriptor->find_value_from_name("count") == nullptr);
			}
		}
	}

	SECTION("Clearing Tasks")
	{
		loupe::clear_reflect_tasks();
		ref = loupe::reflect();

		CHECK(ref.enums.empty());
		CHECK(ref.types.empty());
		CHECK(ref.type_handlers.empty());
	}
}
