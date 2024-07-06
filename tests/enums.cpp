#include "catch/catch.hpp"
#include "loupe/loupe.h"
#include "loupe/metadata.h"
#include "test_data.h"

TEST_CASE("Reflection Tests - Enums")
{
	loupe::reflection_blob ref = loupe::reflect();

	SECTION("Small-Enum")
	{
		const loupe::type* descriptor = ref.find("small_enum");
		REQUIRE(descriptor);
		CHECK(descriptor->name == "small_enum");
		CHECK(descriptor == ref.find<small_enum>());
		CHECK(descriptor != ref.find<nested::small_enum>());

		REQUIRE(std::holds_alternative<loupe::enumeration>(descriptor->data));
		const loupe::enumeration& data = std::get<loupe::enumeration>(descriptor->data);

		REQUIRE(data.entries.size() == 3);
		CHECK(data.entries[0].name == "local_space");
		CHECK(data.entries[0].value == 0);
		CHECK(data.entries[0].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[1].name == "world_space");
		CHECK(data.entries[1].value == 1);
		CHECK(data.entries[1].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[2].name == "COUNT");
		CHECK(data.entries[2].value == 2);
		CHECK(data.entries[2].metadata.has<loupe::metadata::hidden>() == true);

		REQUIRE(data.find_name(0));
		REQUIRE(data.find_name(1));
		REQUIRE(data.find_name(2));
		CHECK(*data.find_name(0) == "local_space");
		CHECK(*data.find_name(1) == "world_space");
		CHECK(*data.find_name(2) == "COUNT");
		CHECK(data.find_name(3) == nullptr);

		REQUIRE(data.find_value("local_space"));
		REQUIRE(data.find_value("world_space"));
		REQUIRE(data.find_value("COUNT"));
		CHECK(*data.find_value("local_space") == 0);
		CHECK(*data.find_value("world_space") == 1);
		CHECK(*data.find_value("COUNT") == 2);
		CHECK(data.find_value("local") == nullptr);
	}

	SECTION("Nested Small-Enum")
	{
		const loupe::type* descriptor = ref.find("nested::small_enum");
		REQUIRE(descriptor);
		CHECK(descriptor->name == "nested::small_enum");
		CHECK(descriptor != ref.find<small_enum>());
		CHECK(descriptor == ref.find<nested::small_enum>());

		REQUIRE(std::holds_alternative<loupe::enumeration>(descriptor->data));
		const loupe::enumeration& data = std::get<loupe::enumeration>(descriptor->data);

		REQUIRE(data.entries.size() == 3);
		CHECK(data.entries[0].name == "local_space");
		CHECK(data.entries[0].value == 0);
		CHECK(data.entries[0].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[1].name == "world_space");
		CHECK(data.entries[1].value == 1);
		CHECK(data.entries[1].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[2].name == "COUNT");
		CHECK(data.entries[2].value == 2);
		CHECK(data.entries[2].metadata.has<loupe::metadata::hidden>() == true);

		REQUIRE(data.find_name(0));
		REQUIRE(data.find_name(1));
		REQUIRE(data.find_name(2));
		CHECK(*data.find_name(0) == "local_space");
		CHECK(*data.find_name(1) == "world_space");
		CHECK(*data.find_name(2) == "COUNT");
		CHECK(data.find_name(3) == nullptr);

		REQUIRE(data.find_value("local_space"));
		REQUIRE(data.find_value("world_space"));
		REQUIRE(data.find_value("COUNT"));
		CHECK(*data.find_value("local_space") == 0);
		CHECK(*data.find_value("world_space") == 1);
		CHECK(*data.find_value("COUNT") == 2);
		CHECK(data.find_value("local") == nullptr);
	}

	SECTION("Large-Enum")
	{
		const loupe::type* descriptor = ref.find("large_enum");
		REQUIRE(descriptor);
		CHECK(descriptor->name == "large_enum");
		CHECK(descriptor == ref.find<large_enum>());
		CHECK(descriptor != ref.find<nested::large_enum>());

		REQUIRE(std::holds_alternative<loupe::enumeration>(descriptor->data));
		const loupe::enumeration& data = std::get<loupe::enumeration>(descriptor->data);

		REQUIRE(data.entries.size() == 11);
		CHECK(data.entries[0].name == "value0");
		CHECK(data.entries[0].value == 0);
		CHECK(data.entries[0].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[1].name == "value1");
		CHECK(data.entries[1].value == 1);
		CHECK(data.entries[1].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[2].name == "value2");
		CHECK(data.entries[2].value == 2);
		CHECK(data.entries[2].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[3].name == "value3");
		CHECK(data.entries[3].value == 3);
		CHECK(data.entries[3].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[4].name == "value4");
		CHECK(data.entries[4].value == 4);
		CHECK(data.entries[4].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[5].name == "value5");
		CHECK(data.entries[5].value == 5);
		CHECK(data.entries[5].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[6].name == "value6");
		CHECK(data.entries[6].value == 6);
		CHECK(data.entries[6].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[7].name == "value7");
		CHECK(data.entries[7].value == 7);
		CHECK(data.entries[7].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[8].name == "value8");
		CHECK(data.entries[8].value == 8);
		CHECK(data.entries[8].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[9].name == "value9");
		CHECK(data.entries[9].value == 9);
		CHECK(data.entries[9].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[10].name == "COUNT");
		CHECK(data.entries[10].value == 10);
		CHECK(data.entries[10].metadata.has<loupe::metadata::hidden>() == true);

		REQUIRE(data.find_name(0));
		REQUIRE(data.find_name(1));
		REQUIRE(data.find_name(2));
		REQUIRE(data.find_name(3));
		REQUIRE(data.find_name(4));
		REQUIRE(data.find_name(5));
		REQUIRE(data.find_name(6));
		REQUIRE(data.find_name(7));
		REQUIRE(data.find_name(8));
		REQUIRE(data.find_name(9));
		REQUIRE(data.find_name(10));
		CHECK(*data.find_name(0) == "value0");
		CHECK(*data.find_name(1) == "value1");
		CHECK(*data.find_name(2) == "value2");
		CHECK(*data.find_name(3) == "value3");
		CHECK(*data.find_name(4) == "value4");
		CHECK(*data.find_name(5) == "value5");
		CHECK(*data.find_name(6) == "value6");
		CHECK(*data.find_name(7) == "value7");
		CHECK(*data.find_name(8) == "value8");
		CHECK(*data.find_name(9) == "value9");
		CHECK(*data.find_name(10) == "COUNT");
		CHECK(data.find_name(11) == nullptr);

		REQUIRE(data.find_value("value0"));
		REQUIRE(data.find_value("value1"));
		REQUIRE(data.find_value("value2"));
		REQUIRE(data.find_value("value3"));
		REQUIRE(data.find_value("value4"));
		REQUIRE(data.find_value("value5"));
		REQUIRE(data.find_value("value6"));
		REQUIRE(data.find_value("value7"));
		REQUIRE(data.find_value("value8"));
		REQUIRE(data.find_value("value9"));
		REQUIRE(data.find_value("COUNT"));
		CHECK(*data.find_value("value0") == 0);
		CHECK(*data.find_value("value1") == 1);
		CHECK(*data.find_value("value2") == 2);
		CHECK(*data.find_value("value3") == 3);
		CHECK(*data.find_value("value4") == 4);
		CHECK(*data.find_value("value5") == 5);
		CHECK(*data.find_value("value6") == 6);
		CHECK(*data.find_value("value7") == 7);
		CHECK(*data.find_value("value8") == 8);
		CHECK(*data.find_value("value9") == 9);
		CHECK(*data.find_value("COUNT") == 10);
		CHECK(data.find_value("count") == nullptr);
	}

	SECTION("Nested Large-Enum")
	{
		const loupe::type* descriptor = ref.find("nested::large_enum");
		REQUIRE(descriptor);
		CHECK(descriptor->name == "nested::large_enum");
		CHECK(descriptor != ref.find<large_enum>());
		CHECK(descriptor == ref.find<nested::large_enum>());

		REQUIRE(std::holds_alternative<loupe::enumeration>(descriptor->data));
		const loupe::enumeration& data = std::get<loupe::enumeration>(descriptor->data);

		REQUIRE(data.entries.size() == 11);
		CHECK(data.entries[0].name == "value0");
		CHECK(data.entries[0].value == 0);
		CHECK(data.entries[0].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[1].name == "value1");
		CHECK(data.entries[1].value == 1);
		CHECK(data.entries[1].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[2].name == "value2");
		CHECK(data.entries[2].value == 2);
		CHECK(data.entries[2].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[3].name == "value3");
		CHECK(data.entries[3].value == 3);
		CHECK(data.entries[3].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[4].name == "value4");
		CHECK(data.entries[4].value == 4);
		CHECK(data.entries[4].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[5].name == "value5");
		CHECK(data.entries[5].value == 5);
		CHECK(data.entries[5].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[6].name == "value6");
		CHECK(data.entries[6].value == 6);
		CHECK(data.entries[6].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[7].name == "value7");
		CHECK(data.entries[7].value == 7);
		CHECK(data.entries[7].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[8].name == "value8");
		CHECK(data.entries[8].value == 8);
		CHECK(data.entries[8].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[9].name == "value9");
		CHECK(data.entries[9].value == 9);
		CHECK(data.entries[9].metadata.has<loupe::metadata::hidden>() == false);
		CHECK(data.entries[10].name == "COUNT");
		CHECK(data.entries[10].value == 10);
		CHECK(data.entries[10].metadata.has<loupe::metadata::hidden>() == true);

		REQUIRE(data.find_name(0));
		REQUIRE(data.find_name(1));
		REQUIRE(data.find_name(2));
		REQUIRE(data.find_name(3));
		REQUIRE(data.find_name(4));
		REQUIRE(data.find_name(5));
		REQUIRE(data.find_name(6));
		REQUIRE(data.find_name(7));
		REQUIRE(data.find_name(8));
		REQUIRE(data.find_name(9));
		REQUIRE(data.find_name(10));
		CHECK(*data.find_name(0) == "value0");
		CHECK(*data.find_name(1) == "value1");
		CHECK(*data.find_name(2) == "value2");
		CHECK(*data.find_name(3) == "value3");
		CHECK(*data.find_name(4) == "value4");
		CHECK(*data.find_name(5) == "value5");
		CHECK(*data.find_name(6) == "value6");
		CHECK(*data.find_name(7) == "value7");
		CHECK(*data.find_name(8) == "value8");
		CHECK(*data.find_name(9) == "value9");
		CHECK(*data.find_name(10) == "COUNT");
		CHECK(data.find_name(11) == nullptr);

		REQUIRE(data.find_value("value0"));
		REQUIRE(data.find_value("value1"));
		REQUIRE(data.find_value("value2"));
		REQUIRE(data.find_value("value3"));
		REQUIRE(data.find_value("value4"));
		REQUIRE(data.find_value("value5"));
		REQUIRE(data.find_value("value6"));
		REQUIRE(data.find_value("value7"));
		REQUIRE(data.find_value("value8"));
		REQUIRE(data.find_value("value9"));
		REQUIRE(data.find_value("COUNT"));
		CHECK(*data.find_value("value0") == 0);
		CHECK(*data.find_value("value1") == 1);
		CHECK(*data.find_value("value2") == 2);
		CHECK(*data.find_value("value3") == 3);
		CHECK(*data.find_value("value4") == 4);
		CHECK(*data.find_value("value5") == 5);
		CHECK(*data.find_value("value6") == 6);
		CHECK(*data.find_value("value7") == 7);
		CHECK(*data.find_value("value8") == 8);
		CHECK(*data.find_value("value9") == 9);
		CHECK(*data.find_value("COUNT") == 10);
		CHECK(data.find_value("count") == nullptr);
	}
}
