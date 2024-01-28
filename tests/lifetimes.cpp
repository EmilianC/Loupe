#include "catch/catch.hpp"
#include "loupe/loupe.h"
#include "test_data.h"

TEST_CASE("Reflection Tests - Lifetimes")
{
	loupe::reflection_blob ref = loupe::reflect();

	const loupe::type* descriptor = ref.find("nested::base_object");
	REQUIRE(descriptor);

	const auto& structure = std::get<loupe::structure>(descriptor->data);
	const loupe::member* mem_name = structure.find_member("name");
	REQUIRE(mem_name);

	SECTION("Dynamic Storage")
	{
		constexpr std::size_t storage_count = 10;
		std::byte* storage = static_cast<std::byte*>(_aligned_malloc(descriptor->size * storage_count, descriptor->alignment));

		SECTION("Default Construct")
		{
			REQUIRE(descriptor->default_construct_at);

			for (std::size_t i = 0; i < storage_count; ++i)
			{
				descriptor->default_construct_at(storage + i * descriptor->size);
			}

			CHECK(nested::base_object::count == storage_count);
		}

		SECTION("User Constructor")
		{
			REQUIRE(descriptor->user_constructor);

			for (std::size_t i = 0; i < storage_count; ++i)
			{
				std::string name = "obj" + std::to_string(i);

				std::byte* ptr = storage + i * descriptor->size;
				descriptor->user_construct_at(ptr, name);
			}

			REQUIRE(nested::base_object::count == storage_count);

			for (std::size_t i = 0; i < storage_count; ++i)
			{
				const std::string expected_name = "obj" + std::to_string(i);

				const std::byte* ptr = storage + i * descriptor->size;
				const std::string* name_ptr = reinterpret_cast<const std::string*>(ptr + mem_name->offset);

				CHECK(*name_ptr == expected_name);
			}
		}

		for (std::size_t i = 0; i < storage_count; ++i)
		{
			std::byte* ptr = storage + i * descriptor->size;
			descriptor->destruct_at(ptr);
		}
		CHECK(nested::base_object::count == 0);

		_aligned_free(storage);
	}
}
