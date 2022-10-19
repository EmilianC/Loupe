#include "catch/catch.hpp"
#include "loupe/archiver.h"
#include "loupe/loupe.h"
#include "test_data.h"

#include <cereal/archives/json.hpp>

TEST_CASE("Reflection Tests")
{
	loupe::reflection_blob ref = loupe::reflect();

	SECTION("Enums")
	{
		SECTION("Small-Enum")
		{
			const loupe::type* descriptor = ref.find("small_enum");
			REQUIRE(descriptor);
			CHECK(descriptor->name == "small_enum");
			CHECK(descriptor == ref.find<small_enum>());

			REQUIRE(std::holds_alternative<loupe::enum_type>(descriptor->data));
			const loupe::enum_type& data = std::get<loupe::enum_type>(descriptor->data);

			REQUIRE(data.entries.size() == 3);
			CHECK(data.entries[0].name == "local_space");
			CHECK(data.entries[0].value == 0);
			CHECK(data.entries[1].name == "world_space");
			CHECK(data.entries[1].value == 1);
			CHECK(data.entries[2].name == "COUNT");
			CHECK(data.entries[2].value == 2);

			REQUIRE(data.find_enum_name(0));
			REQUIRE(data.find_enum_name(1));
			REQUIRE(data.find_enum_name(2));
			CHECK(*data.find_enum_name(0) == "local_space");
			CHECK(*data.find_enum_name(1) == "world_space");
			CHECK(*data.find_enum_name(2) == "COUNT");
			CHECK(data.find_enum_name(3) == nullptr);

			REQUIRE(data.find_enum_value("local_space"));
			REQUIRE(data.find_enum_value("world_space"));
			REQUIRE(data.find_enum_value("COUNT"));
			CHECK(*data.find_enum_value("local_space") == 0);
			CHECK(*data.find_enum_value("world_space") == 1);
			CHECK(*data.find_enum_value("COUNT") == 2);
			CHECK(data.find_enum_value("local") == nullptr);

			/*
			SECTION("Nested Namespace")
			{
				const loupe::enum_descriptor* nested_descriptor = ref.find_enum("nested::small_enum");
				REQUIRE(nested_descriptor);
				CHECK(nested_descriptor->name == "nested::small_enum");
				CHECK(nested_descriptor == ref.find_enum<nested::small_enum>());
				CHECK(nested_descriptor != ref.find_enum<small_enum>());
				CHECK(nested_descriptor != descriptor);

				REQUIRE(nested_descriptor->entries.size() == 3);
				CHECK(nested_descriptor->entries[0].name == "local_space");
				CHECK(nested_descriptor->entries[0].value == 0);
				CHECK(nested_descriptor->entries[1].name == "world_space");
				CHECK(nested_descriptor->entries[1].value == 1);
				CHECK(nested_descriptor->entries[2].name == "COUNT");
				CHECK(nested_descriptor->entries[2].value == 2);

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
			*/
		}

		/*
		SECTION("Large-Enum")
		{
			const loupe::enum_descriptor* descriptor = ref.find_enum("large_enum");
			REQUIRE(descriptor);
			CHECK(descriptor->name == "large_enum");
			CHECK(descriptor == ref.find_enum<large_enum>());

			REQUIRE(descriptor->entries.size() == 11);
			CHECK(descriptor->entries[0].name == "value0");
			CHECK(descriptor->entries[0].value == 0);
			CHECK(descriptor->entries[1].name == "value1");
			CHECK(descriptor->entries[1].value == 1);
			CHECK(descriptor->entries[2].name == "value2");
			CHECK(descriptor->entries[2].value == 2);
			CHECK(descriptor->entries[3].name == "value3");
			CHECK(descriptor->entries[3].value == 3);
			CHECK(descriptor->entries[4].name == "value4");
			CHECK(descriptor->entries[4].value == 4);
			CHECK(descriptor->entries[5].name == "value5");
			CHECK(descriptor->entries[5].value == 5);
			CHECK(descriptor->entries[6].name == "value6");
			CHECK(descriptor->entries[6].value == 6);
			CHECK(descriptor->entries[7].name == "value7");
			CHECK(descriptor->entries[7].value == 7);
			CHECK(descriptor->entries[8].name == "value8");
			CHECK(descriptor->entries[8].value == 8);
			CHECK(descriptor->entries[9].name == "value9");
			CHECK(descriptor->entries[9].value == 9);
			CHECK(descriptor->entries[10].name == "COUNT");
			CHECK(descriptor->entries[10].value == 10);

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
				CHECK(nested_descriptor == ref.find_enum<nested::large_enum>());
				CHECK(nested_descriptor != ref.find_enum<large_enum>());
				CHECK(nested_descriptor != descriptor);

				REQUIRE(nested_descriptor->entries.size() == 11);
				CHECK(nested_descriptor->entries[0].name == "value0");
				CHECK(nested_descriptor->entries[0].value == 0);
				CHECK(nested_descriptor->entries[1].name == "value1");
				CHECK(nested_descriptor->entries[1].value == 1);
				CHECK(nested_descriptor->entries[2].name == "value2");
				CHECK(nested_descriptor->entries[2].value == 2);
				CHECK(nested_descriptor->entries[3].name == "value3");
				CHECK(nested_descriptor->entries[3].value == 3);
				CHECK(nested_descriptor->entries[4].name == "value4");
				CHECK(nested_descriptor->entries[4].value == 4);
				CHECK(nested_descriptor->entries[5].name == "value5");
				CHECK(nested_descriptor->entries[5].value == 5);
				CHECK(nested_descriptor->entries[6].name == "value6");
				CHECK(nested_descriptor->entries[6].value == 6);
				CHECK(nested_descriptor->entries[7].name == "value7");
				CHECK(nested_descriptor->entries[7].value == 7);
				CHECK(nested_descriptor->entries[8].name == "value8");
				CHECK(nested_descriptor->entries[8].value == 8);
				CHECK(nested_descriptor->entries[9].name == "value9");
				CHECK(nested_descriptor->entries[9].value == 9);
				CHECK(nested_descriptor->entries[10].name == "COUNT");
				CHECK(nested_descriptor->entries[10].value == 10);

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
		*/
	}

	SECTION("Visitors")
	{
		const loupe::type* descriptor = ref.find<game_object>();
		REQUIRE(descriptor);

		bool result = descriptor->visit(
			[](const loupe::class_type& data) { return true; },
			[](const loupe::enum_type& data) { return false; },
			[](const auto& data) { return false; }
		);
		CHECK(result);

		int check = -1;
		descriptor->visit(
			[&](const loupe::class_type& data) { check = 1; },
			[&](const loupe::enum_type& data) { check = 0; },
			[&](const auto& data) { check = 0; }
		);
		CHECK(check == 1);
	}

	/*
	SECTION("Structures")
	{
		const loupe::type_descriptor* descriptor = ref.find_type("mat3");
		REQUIRE(descriptor);

		REQUIRE(descriptor->members.size() == 1);
		CHECK(descriptor->members[0].is_array == true);
		CHECK(descriptor->members[0].element_count == 9);
	}

	SECTION("Serialization")
	{
		std::stringstream stream;
		cereal::JSONOutputArchive archive(stream);

		loupe::archiver archiver(ref, archive);

		game_object object;
		object.enabled = true;
		object.health = 50.0f;
		object.local_transform.position = { 1.0f, 2.0f, 3.0f };
		object.velocity = { 0.0f, 0.0f, 1.0f };

		const loupe::type_descriptor* object_type = ref.find_type<game_object>();

		archiver.serialize(&object, object_type);

		CHECK(object_type);
	}

	SECTION("Clearing Tasks")
	{
		loupe::clear_reflect_tasks();
		ref = loupe::reflect();

		CHECK(ref.enums.empty());
		CHECK(ref.types.empty());
	}*/
}
